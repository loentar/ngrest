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

#ifdef USE_GET_WRITE_QUEUE
#ifndef WIN32
#include <sys/select.h>
#else
#include <winsock2.h>
#endif
#endif

struct sockaddr;

namespace ngrest {

class ClientCallback {
public:
    virtual ~ClientCallback()
    {
    }

    virtual void connected(int fd, const sockaddr* addr) = 0;
    virtual void disconnected(int fd) = 0;
    virtual void error(int fd) = 0;
    virtual bool readyRead(int fd) = 0;
    virtual bool readyWrite(int fd) = 0;
#ifdef USE_GET_WRITE_QUEUE
    virtual const fd_set& getWriteQueue() const = 0;
#endif
};

}

#endif // NGREST_CLIENTCALLBACK_H

