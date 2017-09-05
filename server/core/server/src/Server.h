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

#ifndef NGREST_SERVER_H
#define NGREST_SERVER_H

#ifndef HAS_EPOLL
#ifndef WIN32
#include <sys/select.h>
#else
#include <winsock2.h>
#endif
#endif

#include <queue>
#ifdef NGREST_THREAD_LOCK
#include <mutex>
#include <atomic>
#elif defined DEBUG
#include <thread>
#endif
#include <ngrest/engine/Looper.h>
#include "servercommon.h"
#include "ClientHandler.h"

#ifdef HAS_EPOLL
struct epoll_event;
#endif

namespace ngrest {

/**
 * @brief simple socket server class with support of epoll or select
 */
class Server: public CloseConnectionCallback, public Looper
{
public:
    /**
     * @brief constructor
     */
    Server();

    /**
     * @brief destructor
     */
    ~Server();

    /**
     * @brief create server with arguments
     * @param args arguments to pass to server
     * @return true - server successfully created
     */
    bool create(const StringMap& args);

    /**
     * @brief set callback to handle client events
     * @param callback client callback
     */
    void setClientCallback(ClientCallback* callback);

    /**
     * @brief start server with epoll event loop (or with select)
     * @return server exit status
     */
    int exec();

    /**
     * @brief stops the server
     */
    void quit();

    /**
     * @brief close connection to client
     * @param fd client socket descriptor
     */
    virtual void closeConnection(Socket fd) override;

    /**
     * @brief post task to event loop (main thread).
     * This function is thread-safe if ngrest is compiled with WITH_THREAD_LOCK
     * @param task
     */
    virtual void post(Task task) override;

private:
    Socket createServerSocket(const std::string& ip, const std::string& port);
    bool setupNonblock(Socket fd);
    bool handleIncomingConnection();
    void handleRequest(Socket fd);

private:
    bool isStopping = false;
    Socket fdServer = 0;
#ifdef HAS_EPOLL
    int fdEpoll = 0;
    epoll_event* event = nullptr;
    epoll_event* events = nullptr;
#else
    fd_set activeFds;
    fd_set writeFds;
#endif
    ClientCallback* callback = nullptr;
    std::string ip;
    std::string port;
#ifdef NGREST_THREAD_LOCK
    std::atomic<bool> hasTasks;
    std::mutex mutex;
#elif defined DEBUG
    std::thread::id mainThreadId;
#endif
    std::queue<Task> taskQueue;
};

}

#endif // NGREST_SERVER_H
