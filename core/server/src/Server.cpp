/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifdef HAS_EPOLL
#include <sys/epoll.h>
#endif
#ifndef WIN32
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#else
#include <winsock2.h>
#include <ws2tcpip.h>
#endif
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <ngrest/utils/Log.h>
#include <ngrest/utils/Error.h>
#include <ngrest/utils/Exception.h>

#include "ClientCallback.h"
#include "Server.h"

#define MAXEVENTS 64
#define NGREST_EVENT_LOOP_CHECK_PERIOD 200

#ifndef HAS_EPOLL
#warning No epoll support - switching to compatibility mode
#endif

namespace ngrest {

#ifdef WIN32

#ifndef EWOULDBLOCK
#define EWOULDBLOCK WSAEWOULDBLOCK
#endif

class SocketInitializer
{
public:
    SocketInitializer()
    {
        WSADATA wsaData;
        int res = WSAStartup(MAKEWORD(2, 0), &wsaData);
        if (res) {
            LogError() << "WSAStartup FAILED: " << Error::getError(WSAGetLastError());
            exit(1);
        }
    }

    ~SocketInitializer()
    {
        WSACleanup();
    }
};

static SocketInitializer socketInitializer;
#endif


Server::Server()
#if !defined NGREST_THREAD_LOCK && defined DEBUG
    : mainThreadId(std::this_thread::get_id())
#endif
{
#ifdef HAS_EPOLL
    event = reinterpret_cast<epoll_event*>(calloc(1, sizeof(epoll_event)));

    /* Buffer where events are returned */
    events = reinterpret_cast<epoll_event*>(calloc(MAXEVENTS, sizeof(epoll_event)));
#else
    LogWarning() << "This version compiled without epoll support.";
#endif
}

Server::~Server()
{
    if (fdServer != 0)
        close(fdServer);
#ifdef HAS_EPOLL
    free(events);
    free(event);
#endif
}

bool Server::create(const StringMap& args)
{
    std::string port = "9098";
    std::string ip;

    auto it = args.find("p");
    if (it != args.end())
        port = it->second;

    it = args.find("l");
    if (it != args.end())
        ip = it->second;

    fdServer = createServerSocket(ip, port);
    if (fdServer == NGREST_SOCKET_ERROR)
        return false;

    if (!setupNonblock(fdServer))
        return false;

    Socket res = listen(fdServer, SOMAXCONN);
    if (res == NGREST_SOCKET_ERROR) {
        perror("listen");
        return false;
    }

#ifdef HAS_EPOLL
    fdEpoll = epoll_create1(0);
    if (fdEpoll == -1) {
        perror("epoll_create");
        return false;
    }

    event->data.fd = fdServer;
    event->events = EPOLLIN | EPOLLET;
    res = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, fdServer, event);
    if (res == -1) {
        perror("epoll_ctl");
        return false;
    }
#else
    FD_ZERO(&activeFds);
#endif

    return true;
}

void Server::setClientCallback(ClientCallback* callback)
{
    this->callback = callback;

    if (callback)
        callback->setCloseConnectionCallback(this);
}

int Server::exec()
{
    if (!callback) {
        LogError() << "Client callback is not set!";
        return EXIT_FAILURE;
    }

    std::string host;
    if (ip.empty()) {
        host = "localhost";
    } else if (ip.find(':') != std::string::npos) { // IPv6
        host = "[" + ip + "]";
    } else { // IPv4
        host = ip;
    }

    LogInfo() << "Simple ngrest server started on port " << port << ".";
    LogInfo() << "Deployed services: http://" << host << ":" << port << "/ngrest/services";

#ifndef HAS_EPOLL
    fd_set readFds;
    timeval timeout;

    FD_ZERO(&writeFds);
#endif

    Task task;

    // The event loop
    while (!isStopping) {
#ifdef HAS_EPOLL
        int n = epoll_wait(fdEpoll, events, MAXEVENTS, NGREST_EVENT_LOOP_CHECK_PERIOD);
        for (int i = 0; i < n && !isStopping; ++i) {
            const uint32_t event = events[i].events;
            if ((events[i].events & EPOLLERR) ||
                   (events[i].events & EPOLLHUP) ||
                   (!(events[i].events & (EPOLLIN | EPOLLOUT)))) {
                /* An error has occured on this fd, or the socket is not
                 ready for reading/writing(why were we notified then?) */
                LogError() << "epoll error" << Error::getLastError();
                if (callback)
                    callback->error(events[i].data.fd);
                close(events[i].data.fd);
            } else if (fdServer == events[i].data.fd) {
                /* We have a notification on the listening socket, which
                 means one or more incoming connections. */
                handleIncomingConnection();
            } else {
                /* We have data on the fd waiting to be read. Read and
                 display it. We must read whatever data is available
                 completely, as we are running in edge-triggered mode
                 and won't get a notification again for the same
                 data. */
                if (event & EPOLLIN) {
                    handleRequest(events[i].data.fd);
                } else if (event & EPOLLOUT) {
                    callback->readyWrite(events[i].data.fd);
                } else {
                    LogError() << "Unknown EPOLL event" << Error::getLastError();
                }
            }
        }
#else
        // Block until input arrives on one or more active sockets.
        timeout.tv_sec = 0;
        timeout.tv_usec = NGREST_EVENT_LOOP_CHECK_PERIOD * 1000;
        readFds = activeFds;
        FD_SET(fdServer, &readFds); // add server socket
        int readyFds = select(FD_SETSIZE, &readFds, &writeFds, NULL, &timeout);
        if (readyFds != 0) {
            if (readyFds < 0) {
                if (errno != EINTR)
                    LogError() << "select: " << Error::getLastError();
                return EXIT_FAILURE;
            }

            int processedFds = 0;
            // Service all the sockets with input pending.
            for (int i = 0; (i < FD_SETSIZE) && (processedFds < readyFds); ++i) {
#ifdef WIN32
                Socket fd = readFds.fd_array[i];
#else
                Socket fd = i;
#endif
                if (FD_ISSET(fd, &readFds)) {
                    if (fd == fdServer) {
                        handleIncomingConnection();
                    } else {
                        handleRequest(fd);
                    }
                    ++processedFds;
                }

                if (processedFds == readyFds)
                    break;

#ifdef WIN32
                fd = writeFds.fd_array[i];
#endif
                if (FD_ISSET(fd, &writeFds)) {
                    WriteStatus status = callback->readyWrite(fd);
                    switch (status) {
                    case WriteStatus::Again:
#ifndef HAS_EPOLL
                        FD_SET(fd, &writeFds);
#endif
                        break;

                    case WriteStatus::Done:
#ifndef HAS_EPOLL
                        FD_CLR(fd, &writeFds);
#endif
                        break;

                    case WriteStatus::Close:
                        closeConnection(fd);
                        break;

                    default:
                        break;
                    }
                    ++processedFds;
                }
            }
        }
#endif
#ifdef NGREST_THREAD_LOCK
        if (hasTasks) {
            std::lock_guard<std::mutex> lock(mutex);
            hasTasks = false;
#endif
            while (!taskQueue.empty()) {
                task = taskQueue.front();
                taskQueue.pop();
                task();
            }
#ifdef NGREST_THREAD_LOCK
        }
#endif
    }

    LogInfo() << "Server finished";
    return EXIT_SUCCESS;
}

void Server::quit()
{
    isStopping = true;
}

void Server::closeConnection(Socket fd)
{
#ifndef HAS_EPOLL
    FD_CLR(fd, &activeFds);
    FD_CLR(fd, &writeFds);
#endif

    close(fd);
    callback->disconnected(fd);
}

void Server::post(Task task)
{
#ifdef NGREST_THREAD_LOCK
    std::lock_guard<std::mutex> lock(mutex);
    hasTasks = true;
#else
    NGREST_DEBUG_ASSERT(std::this_thread::get_id() == mainThreadId,
                        "You trying to post task from non-main thread, "
                        "but ngrest is not compiled with WITH_THREAD_LOCK flag."
                        "This may cause undefined behavior and because of that restricted.");
#endif
    taskQueue.push(task);
}

Socket Server::createServerSocket(const std::string& ip, const std::string& port)
{
    struct addrinfo hints;
    struct addrinfo* addr;
    struct addrinfo* curr;
    Socket sock = -1;
    std::string lastError;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */

    int res = getaddrinfo(ip.empty() ? nullptr : ip.c_str(), port.c_str(), &hints, &addr);
    if (res != 0) {
        LogError() << "getaddrinfo: " << gai_strerror(res);
        return -1;
    }

    for (curr = addr; curr != nullptr; curr = curr->ai_next) {
        sock = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
        if (sock == NGREST_SOCKET_ERROR)
            continue;

        int reuse = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, reinterpret_cast<char*>(&reuse), sizeof(reuse));

        res = bind(sock, curr->ai_addr, curr->ai_addrlen);
        if (res == 0) {
            /* We managed to bind successfully! */
            break;
        }

        lastError = Error::getLastError();
        close(sock);
    }

    freeaddrinfo(addr);

    if (curr == nullptr) {
        LogError() << "Could not bind to " << (ip.empty() ? " [all interfaces]" : ip.c_str()) << " : " << port
                   << ": " << lastError;
        return -1;
    }

    this->ip = ip;
    this->port = port;

    return sock;
}


bool Server::setupNonblock(Socket fd)
{
#ifndef WIN32
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        LogError() << "Error getting socket flags" << Error::getLastError();
        return false;
    }

    flags |= O_NONBLOCK;
    int res = fcntl(fd, F_SETFL, flags);
    if (res == -1) {
        LogError() << "Error setting socket flags" << Error::getLastError();
        return false;
    }
#else
    u_long ulVal = 1;
    int res = ioctlsocket(fd, FIONBIO, &ulVal);
    if (res != 0) {
        LogError() << Error::getError(WSAGetLastError());
        return false;
    }
#endif

    return true;
}

bool Server::handleIncomingConnection()
{
    while (!isStopping) {
        sockaddr_storage inAddr;
        socklen_t inLen = sizeof(inAddr);
        Socket fdIn = accept(fdServer, reinterpret_cast<sockaddr*>(&inAddr), &inLen);
        if (fdIn == NGREST_SOCKET_ERROR) {
#ifndef WIN32
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
#else
            int err = WSAGetLastError();
            if (err == WSATRY_AGAIN || err == WSAEWOULDBLOCK) {
#endif
                // We have processed all incoming connections.
                break;
            } else {
                LogError() << "accept: " << Error::getLastError();
                break;
            }
        }

        // Make the incoming socket non-blocking
        int res = setupNonblock(fdIn);
        if (res == -1) {
            close(fdIn);
            continue;
        }

        int nodelayOpt = 1;
        setsockopt(fdIn, IPPROTO_TCP, TCP_NODELAY, reinterpret_cast<const char*>(&nodelayOpt),
                   sizeof(nodelayOpt));

        // add it to the list of fds to monitor.
#ifdef HAS_EPOLL
        event->data.fd = fdIn;
        event->events = EPOLLIN | EPOLLOUT | EPOLLET;
        res = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, fdIn, event);
        if (res == -1)
            LogError() << "Failed to add client " << Error::getLastError();
#else
        FD_SET(fdIn, &activeFds);
#endif

        callback->connected(fdIn, &inAddr);
    }

    return true;
}

void Server::handleRequest(Socket fd)
{
#ifndef WIN32
    int64_t bytesAvail = 0;
    int res = ioctl(fd, FIONREAD, &bytesAvail);
#else
    u_long bytesAvail = 0;
    int res = ioctlsocket(fd, FIONREAD, &bytesAvail);
#endif

    // if res = 0, the query is ok, trust bytesAvail
    // else if there are some bytes to read or the query is failed - we will try to read
    if (res || bytesAvail) {
        try {
            if (callback->readyRead(fd))
                return;
        } NGREST_CATCH_ALL {
            // close client connection
        }
    }

    closeConnection(fd);
}

}
