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

#ifdef USE_GET_WRITE_QUEUE
#ifndef WIN32
#include <sys/select.h>
#else
#include <winsock2.h>
#endif
#endif

#include "servercommon.h"

#ifdef HAS_EPOLL
struct epoll_event;
#endif

namespace ngrest {

class ClientCallback;

class Server
{
public:
    Server();
    ~Server();

    bool create(const StringMap& args);

    void setClientCallback(ClientCallback* callback);

    int exec();

    void quit();

private:
    int createServerSocket(const std::string& port);
    bool setupNonblock(int fd);
    bool handleIncomingConnection();
    void handleRequest(int fd);

private:
    bool isStopping = false;
    int fdServer = 0;
    int fdEpoll = 0;
#ifdef HAS_EPOLL
    epoll_event* event = nullptr;
    epoll_event* events = nullptr;
#else
    fd_set activeFds;
#endif
    ClientCallback* callback = nullptr;
    std::string port;
};

}

#endif // NGREST_SERVER_H
