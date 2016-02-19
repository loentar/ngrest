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
#include <sys/socket.h>
#include <sys/epoll.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <ngrest/utils/Log.h>

#include "ClientCallback.h"
#include "Server.h"

#define MAXEVENTS 64

namespace ngrest {

Server::Server()
{
    event = reinterpret_cast<epoll_event*>(calloc(1, sizeof(epoll_event)));

    /* Buffer where events are returned */
    events = reinterpret_cast<epoll_event*>(calloc(MAXEVENTS, sizeof(epoll_event)));
}

Server::~Server()
{
    if (fdServer != 0)
        close(fdServer);
    free(events);
    free(event);
}

bool Server::create(const StringMap& args)
{
    std::string port = "9098";
    auto it = args.find("p");
    if (it != args.end())
        port = it->second;

    fdServer = createServerSocket(port);
    if (fdServer == -1)
        return false;

    if (!setupNonblock(fdServer))
        return false;

    int res = listen(fdServer, SOMAXCONN);
    if (res == -1) {
        perror("listen");
        return false;
    }

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

    return true;
}

void Server::setClientCallback(ClientCallback* callback)
{
    this->callback = callback;
}

int Server::exec()
{
    if (!callback) {
        LogError() << "Client callback is not set!";
        return EXIT_FAILURE;
    }

    LogInfo() << "Simple ngrest server started on port " << port << ".";
    LogInfo() << "Deployed services: http://localhost:" << port << "/ngrest/services";

    /* The event loop */
    while (!isStopping) {
        int n = epoll_wait(fdEpoll, events, MAXEVENTS, -1);
        for (int i = 0; i < n && !isStopping; ++i) {
            const uint32_t event = events[i].events;
            if ((events[i].events & EPOLLERR) ||
                   (events[i].events & EPOLLHUP) ||
                   (!(events[i].events & (EPOLLIN | EPOLLOUT)))) {
                /* An error has occured on this fd, or the socket is not
                 ready for reading/writing(why were we notified then?) */
                LogError() << "epoll error";
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
                    LogError() << "Unknown EPOLL event";
                }
            }
        }
    }

    LogInfo() << "Server finished";
    return EXIT_SUCCESS;
}

void Server::quit()
{
    isStopping = true;
}

int Server::createServerSocket(const std::string& port)
{
    struct addrinfo hints;
    struct addrinfo* addr;
    struct addrinfo* curr;
    int sock = -1;

    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
    hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
    hints.ai_flags = AI_PASSIVE;     /* All interfaces */

    int res = getaddrinfo(nullptr, port.c_str(), &hints, &addr);
    if (res != 0) {
        LogError() << "getaddrinfo: " << gai_strerror(res);
        return -1;
    }

    for (curr = addr; curr != nullptr; curr = curr->ai_next) {
        sock = socket(curr->ai_family, curr->ai_socktype, curr->ai_protocol);
        if (sock == -1)
            continue;

        int i = 1;
        setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char*) &i, sizeof(i));

        res = bind(sock, curr->ai_addr, curr->ai_addrlen);
        if (res == 0) {
            /* We managed to bind successfully! */
            break;
        }

        close(sock);
    }

    freeaddrinfo(addr);

    if (curr == nullptr) {
        LogError() << "Could not bind to port " << port;
        return -1;
    }

    this->port = port;

    return sock;
}


bool Server::setupNonblock(int fd)
{
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        return false;
    }

    flags |= O_NONBLOCK;
    int res = fcntl(fd, F_SETFL, flags);
    if (res == -1) {
        perror("fcntl");
        return false;
    }

    return true;
}

bool Server::handleIncomingConnection()
{
    while (!isStopping) {
        struct sockaddr inAddr;
        socklen_t inLen = sizeof(inAddr);
        int fdIn = accept(fdServer, &inAddr, &inLen);
        if (fdIn == -1) {
            if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                /* We have processed all incoming connections. */
                break;
            } else {
                perror("accept");
                break;
            }
        }

        /* Make the incoming socket non-blocking */
        int res = setupNonblock(fdIn);
        if (res == -1) {
            close(fdIn);
            continue;
        }

        int nodelayOpt = 1;
        setsockopt(fdIn, IPPROTO_TCP, TCP_NODELAY, &nodelayOpt, sizeof(nodelayOpt));

        /* add it to the list of fds to monitor. */
        event->data.fd = fdIn;
        event->events = EPOLLIN | EPOLLOUT | EPOLLET;
        res = epoll_ctl(fdEpoll, EPOLL_CTL_ADD, fdIn, event);
        if (res == -1)
            perror("epoll_ctl");

        callback->connected(event->data.fd, &inAddr);
    }

    return true;
}

void Server::handleRequest(int fd)
{
    int64_t bytesAvail = 0;
    int res = ioctl(fd, FIONREAD, &bytesAvail);
    // ioctlsocket(socket, FIONREAD, &bytesAvail)

    // if res = 0, the query is ok, trust bytesAvail
    // else if there are some bytes to read or the query is failed - we will try to read
    if (res || bytesAvail) {
        if (callback->readyRead(fd))
            return;
    } else {
        LogDebug() << "client #" << fd << " closed connection";
    }

    close(fd); // disconnect client in case of errors
    callback->disconnected(fd);
}

}
