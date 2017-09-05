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

#ifndef NGREST_CLIENTCALLBACK_H
#define NGREST_CLIENTCALLBACK_H

#ifndef HAS_EPOLL
#ifndef WIN32
#include <sys/select.h>
#else
#include <winsock2.h>
#endif
#endif

#include "servercommon.h"

struct sockaddr_storage;

namespace ngrest {

/**
 * @brief status code
 */
enum class Status {
    Success, //!< no action
    Done,    //!< response finished, remove socket from write queue
    Again,   //!< response processedd partially, add socket to write queue
    Close    //!< close client connection
};

/**
 * @brief callback to close connections from handler
 */
class CloseConnectionCallback {
public:
    /**
     * @brief virtual destructor
     */
    virtual ~CloseConnectionCallback() {}

    /**
     * @brief close connection to client
     * @param fd client socket descriptor
     */
    virtual void closeConnection(Socket fd) = 0;
};

/**
 * @brief callback class to process events from client
 */
class ClientCallback {
public:
    /**
     * @brief virtual destructor
     */
    virtual ~ClientCallback()
    {
    }

    /**
     * @brief client connected event
     * @param fd client socket descriptor
     * @param addr client address
     */
    virtual void connected(Socket fd, const sockaddr_storage* addr) = 0;

    /**
     * @brief client disconnected event
     * @param fd client socket descriptor
     */
    virtual void disconnected(Socket fd) = 0;

    /**
     * @brief client error event
     * @param fd client socket descriptor
     */
    virtual void error(Socket fd) = 0;

    /**
     * @brief data available from client
     * @param fd client socket descriptor
     * @return true - read success, false - close connection
     */
    virtual bool readyRead(Socket fd) = 0;

    /**
     * @brief client socket is ready for writing
     * @param fd client socket descriptor
     * @return write status code
     */
    virtual Status readyWrite(Socket fd) = 0;

    /**
     * @brief set calback to close connection
     * @param callback callback
     */
    virtual void setCloseConnectionCallback(CloseConnectionCallback* callback) = 0;
};

}

#endif // NGREST_CLIENTCALLBACK_H

