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

/**
 * @brief Client handler. Manages clients messages
 */
class ClientHandler: public ClientCallback
{
public:
    /**
     * @brief constructor
     * @param engine engine to work with
     * @param transport transport to work with
     */
    ClientHandler(Engine& engine, Transport& transport);

    /**
     * @brief destructor
     */
    ~ClientHandler();

    /**
     * @brief client connected event
     * @param fd client socket descriptor
     * @param addr client address
     */
    virtual void connected(Socket fd, const sockaddr_storage* addr) override;

    /**
     * @brief client disconnected event
     * @param fd client socket descriptor
     */
    virtual void disconnected(Socket fd) override;

    /**
     * @brief client error event
     * @param fd client socket descriptor
     */
    virtual void error(Socket fd) override;

    /**
     * @brief data available from client
     * @param fd client socket descriptor
     * @return true - read success, false - close connection
     */
    virtual bool readyRead(Socket fd) override;

    /**
     * @brief client socket is ready for writing
     * @param fd client socket descriptor
     * @return write status code
     */
    virtual WriteStatus readyWrite(Socket fd) override;

    /**
     * @brief set calback to close connection
     * @param callback callback
     */
    virtual void setCloseConnectionCallback(CloseConnectionCallback* callback) override;

    /**
     * @brief parse http header from buffer
     * @param buffer mutable buffer which stores http header
     * @param messageData message data to write header to
     */
    void parseHttpHeader(char* buffer, MessageData* messageData);

    /**
     * @brief prepare and process received request from client
     * @param clientFd client socket descriptor
     * @param messageData client message data
     */
    void processRequest(Socket clientFd, MessageData* messageData);

    /**
     * @brief build response and send it to client
     * @param clientFd client socket descriptor
     * @param messageData client message data
     */
    void processResponse(Socket clientFd, MessageData* messageData);

    /**
     * @brief build error response and send it to client
     * @param clientFd client socket descriptor
     * @param messageData client message data
     * @param error error description
     */
    void processError(Socket clientFd, MessageData* messageData, const Exception& error);

private:
    WriteStatus writeNextPart(Socket clientFd, ClientInfo* clientInfo, MessageData* messageData);

private:
    uint64_t lastId = 0;
    std::unordered_map<Socket, ClientInfo*> clients;
    Engine& engine;
    Transport& transport;
    MemPooler* pooler;
    CloseConnectionCallback* closeCallback = nullptr;
};

}

#endif // CLIENTHANDLER_H
