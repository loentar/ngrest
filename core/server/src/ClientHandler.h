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

#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <unordered_map>

#include "ClientCallback.h"

namespace ngrest {

class Exception;
class Engine;
class Transport;
class MemPooler;
struct MessageData;
struct ClientInfo;

class ClientHandler: public ClientCallback
{
public:
    ClientHandler(Engine& engine, Transport& transport);
    ~ClientHandler();

    virtual void connected(int fd, const sockaddr* addr) override;
    virtual void disconnected(int fd) override;
    virtual void error(int fd) override;
    virtual bool readyRead(int fd) override;
    virtual bool readyWrite(int fd) override;
#ifdef USE_GET_WRITE_QUEUE
    inline virtual const fd_set& getWriteQueue() const override {
        return writeQueue;
    }
#endif

    void parseHttpHeader(char* buffer, MessageData* messageData);
    void processRequest(int clientFd, MessageData* messageData);
    void processResponse(int clientFd, MessageData* messageData);
    void processError(int clientFd, MessageData* messageData, const Exception& error);

private:
    bool writeNextPart(int clientFd, ClientInfo* clientInfo, MessageData* messageData);

private:
    uint64_t lastId = 0;
    std::unordered_map<int, ClientInfo*> clients;
    Engine& engine;
    Transport& transport;
    MemPooler* pooler;
#ifdef USE_GET_WRITE_QUEUE
    fd_set writeQueue;
#endif
};

}

#endif // CLIENTHANDLER_H
