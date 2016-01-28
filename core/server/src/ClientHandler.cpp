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

#include <unistd.h>
#include <error.h>
#include <sys/socket.h>
#include <netdb.h>

#include <list>

#include <ngrest/utils/Log.h>
#include <ngrest/utils/MemPool.h>
#include <ngrest/utils/Exception.h>
#include <ngrest/utils/fromcstring.h>
#include <ngrest/utils/tocstring.h>
#include <ngrest/utils/ElapsedTimer.h>
#include <ngrest/common/Message.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/HttpException.h>
#include <ngrest/engine/Engine.h>

#include "strutils.h"
#include "ClientHandler.h"

#define TRY_BLOCK_SIZE 512
#define MAX_REQUEST_SIZE 10485760 // 10 Mb

namespace ngrest {

static const uint64_t INVALID_VALUE = static_cast<uint64_t>(-1);

struct MessageData
{
    ElapsedTimer timer;
    uint64_t id;
    MessageContext context;
    MemPool poolStr;
    MemPool poolBody;
    bool usePoolBody = false;
    bool isChunked = false;
    uint64_t contentLength = INVALID_VALUE;
    uint64_t httpBodyOffset = 0;
    uint64_t httpBodyRemaining = INVALID_VALUE;
//    uint64_t currentChunkRemaining = 0;
    bool processing = false;

    MessageData(uint64_t id_, Transport* transport, Engine* engine):
        timer(true),
        id(id_),
        poolStr(2048),
        poolBody(1024)
    {
        LogDebug() << "Start handling request " << id;
        context.engine = engine;
        context.transport = transport;
        context.request = context.pool.alloc<HttpRequest>();
        context.response = context.pool.alloc<HttpResponse>();
    }
};

struct ClientInfo
{
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    bool deleteLater = false;

    std::list<MessageData*> requests;
};


class ClientHandlerCallback: public MessageCallback
{
public:
    ClientHandlerCallback(ClientHandler* handler_, int clientFd_, MessageData* messageData_):
        handler(handler_), clientFd(clientFd_), messageData(messageData_)
    {
    }

    void success()
    {
        handler->processResponse(clientFd, messageData);
    }

    void error(const Exception& error)
    {
        handler->processError(clientFd, messageData, error);
    }

    ClientHandler* handler;
    int clientFd;
    MessageData* messageData;
};


ClientHandler::ClientHandler(Engine& engine_, Transport& transport_):
    engine(engine_), transport(transport_)
{
}

void ClientHandler::connected(int fd, const sockaddr* addr)
{
    ClientInfo*& client = clients[fd];
    if (client == nullptr) {
        client = new ClientInfo();

        int res = getnameinfo(addr, sizeof(*addr),
                              client->host, sizeof(client->host),
                              client->port, sizeof(client->port),
                              NI_NUMERICHOST | NI_NUMERICSERV);
        if (res == 0) {
            LogInfo() << "Accepted connection on client " << fd
                      << " (host=" << client->host << ", port=" << client->port << ")";
        } else {
            client->host[0] = '\0';
            client->port[0] = '\0';
        }
    } else {
        LogError() << "Client #" << fd << " is already connected";
    }
}

void ClientHandler::disconnected(int fd)
{
    ClientInfo* clientInfo = clients[fd];
    if (clientInfo->requests.empty()) {
        delete clientInfo;
    } else {
        clientInfo->deleteLater = true;
    }
    clients.erase(fd);
}

void ClientHandler::error(int fd)
{
    LogError() << "Error client #" << fd;
}

bool ClientHandler::readyRead(int fd)
{
    ClientInfo* clientData = clients[fd];
    if (clientData == nullptr) {
        LogError() << "failed to find client #" << fd;
        return false;
    }

    MessageData* messageData;

    if (clientData->requests.empty()) {
        messageData = new MessageData(++lastId, &transport, &engine);
        clientData->requests.push_back(messageData);
    } else {
        messageData = clientData->requests.back();
        NGREST_ASSERT_NULL(messageData);
        if (messageData->processing) {
            // request is finished to read and now processing.
            // creating new request
            messageData = new MessageData(++lastId, &transport, &engine);
            clientData->requests.push_back(messageData);
        }
    }

    HttpRequest* httpRequest = static_cast<HttpRequest*>(messageData->context.request);
    MemPool& pool = messageData->usePoolBody ? messageData->poolBody : messageData->poolStr;
    for (;;) {
        uint64_t prevSize = pool.getSize();
        uint64_t sizeToRead = (messageData->httpBodyRemaining != INVALID_VALUE)
                ? messageData->httpBodyRemaining : TRY_BLOCK_SIZE;
        char* buffer = pool.grow(sizeToRead);
        ssize_t count = ::read(fd, buffer, sizeToRead);
        if (count == 0) {
            // this should only happen in case of ioctl(fd, FIONREAD...) failure
            // EOF. The remote has closed the connection.
            LogDebug() << "client #" << fd << " closed connection";
            return false;
        }

        if (count == -1) {
            // some error
            if (errno != EAGAIN) {
                LogError() << "failed to read block from client #" << fd
                           << ": " << strerror(errno);
                return false;
            }

            // all available data read
            pool.shrinkLastChunk(sizeToRead);
            break;
        }

        if (count < static_cast<int64_t>(sizeToRead))
            pool.shrinkLastChunk(sizeToRead - count);

        if (messageData->httpBodyRemaining != INVALID_VALUE)
            messageData->httpBodyRemaining -= count;

        if (messageData->httpBodyOffset == 0) {
            MemPool::Chunk* chunk = pool.flatten(false);
            // pool == poolStr until header is not fully read
            buffer = chunk->buffer + prevSize;

            const char* startFind;
            uint64_t searchSize;
            if (prevSize) {
                startFind = buffer - 3;
                searchSize = count + 3;
            } else {
                startFind = buffer;
                searchSize = count;
            }

            const char* pos = strnstr(startFind, "\r\n\r\n", searchSize);
            if (pos) {
                uint64_t httpHeaderSize = prevSize + (pos - buffer);
                messageData->httpBodyOffset = httpHeaderSize + 4;

                chunk->buffer[httpHeaderSize + 2] = '\0'; // terminate HTTP header

                // parse HTTP header
                parseHttpHeader(chunk->buffer, messageData);

                const Header* headerEncoding = httpRequest->getHeader("transfer-encoding");
                if (headerEncoding && !strcmp(headerEncoding->value, "chunked")) {
                    // FIXME: add support for chunked encoding
                    NGREST_THROW_ASSERT("Chunked encoding is not yet supported");
/*
 HTTP/1.1 200 OK
 Server: nginx/1.0.4
 Date: Thu, 06 Oct 2011 16:14:01 GMT
 Content-Type: text/html
 Transfer-Encoding: chunked
 Connection: keep-alive
 Vary: Accept-Encoding
 X-Powered-By: PHP/5.3.6

 23
 This is the data in the first chunk
 1A
 and this is the second one
 3
 con
 8
 sequence
 0
 */
                    messageData->isChunked = true;
                } else {
                    const Header* headerLength = httpRequest->getHeader("content-length");
                    if (headerLength) {
                        NGREST_ASSERT(fromCString(headerLength->value, messageData->contentLength),
                                      "Failed to get content length of request");
                        NGREST_ASSERT(messageData->contentLength < MAX_REQUEST_SIZE,
                                      "Request is too large!");
                        const uint64_t totalRequestLength = messageData->httpBodyOffset +
                                messageData->contentLength;
                        messageData->httpBodyRemaining = totalRequestLength - pool.getSize();
                        messageData->poolBody.reserve(totalRequestLength + 1);

                        // if we didn't receive the whole body yet
                        // store received part of body to another pool to avoid
                        // Header* pointers damage on poolStr->flatten
                        if (messageData->httpBodyRemaining) {
                            // copy already received part of data to poolBody
                            messageData->poolBody.putData(chunk->buffer + messageData->httpBodyOffset,
                                                          chunk->size - messageData->httpBodyOffset);
                            messageData->usePoolBody = true;
                        }
                    } else {
                        // message has no body and ready to process now
                        messageData->httpBodyRemaining = 0;
                    }
                }

            }
        }

        if (messageData->httpBodyRemaining == 0) {
            httpRequest->clientHost = clientData->host;
            httpRequest->clientPort = clientData->port;
            processRequest(fd, messageData);
            break;
        }
    }

    return true;
}

void ClientHandler::parseHttpHeader(char* buffer, MessageData* messageData)
{
    char* curr = buffer;

    // parse method

    const char* method = token(curr);
    NGREST_ASSERT(method >= buffer, "Failed to get HTTP method");
    HttpRequest* httpRequest = static_cast<HttpRequest*>(messageData->context.request);
    NGREST_ASSERT_NULL(httpRequest);

    if (!strcmp(method, "POST")) {
        httpRequest->method = HttpMethod::POST;
    } else if (!strcmp(method, "GET")) {
        httpRequest->method = HttpMethod::GET;
    } else if (!strcmp(method, "PUT")) {
        httpRequest->method = HttpMethod::PUT;
    } else if (!strcmp(method, "DELETE")) {
        httpRequest->method = HttpMethod::DELETE;
    } else {
        httpRequest->method = HttpMethod::UNKNOWN;
    }
    httpRequest->methodStr = method;


    // parse request url

    skipWs(curr);
    httpRequest->path = token(curr);
    NGREST_ASSERT(httpRequest->path > buffer, "Failed to get request URL");

    // seek to the first http header
    NGREST_ASSERT(seekTo(curr, '\n'), "Failed to seek to first HTTP header");
    skipWs(curr);


    // read http headers

    Header* lastHeader = nullptr;
    while (*curr != '\0') {
        char* name = token(curr, ':');
        NGREST_ASSERT(*curr, "Failed to parse HTTP header: unable to read name");
        trimRight(name, curr - 2);
        toLowerCase(name);
        skipWs(curr);
        NGREST_ASSERT(*curr, "Failed to parse HTTP header: unable to read value");
        char* value = token(curr, '\n');
        trimRight(value, curr - 2);
        Header* header = messageData->context.pool.alloc<Header>();
        header->name = name;
        header->value = value;
        if (lastHeader == nullptr) {
            httpRequest->headers = header;
        } else {
            lastHeader->next = header;
        }
        lastHeader = header;
    }
}

void ClientHandler::processRequest(int clientFd, MessageData* messageData)
{
    messageData->processing = true;

    HttpRequest* httpRequest = static_cast<HttpRequest*>(messageData->context.request);
    NGREST_ASSERT_NULL(httpRequest);

    if (messageData->usePoolBody) {
        // handle body from poolBody with zero offset
        MemPool::Chunk* chunk = messageData->poolStr.flatten();
        httpRequest->bodySize = chunk->size;
        httpRequest->body = chunk->buffer;
    } else {
        // handle body from poolStr with offset
        NGREST_ASSERT_NULL(messageData->poolStr.getChunkCount() == 1); // should never happen

        const MemPool::Chunk* chunk = messageData->poolStr.getLastChunk(); // already flat
        NGREST_ASSERT(messageData->httpBodyOffset <= chunk->size, "Request > size");
        httpRequest->bodySize = chunk->size - messageData->httpBodyOffset;
        if (httpRequest->bodySize)
            httpRequest->body = chunk->buffer + messageData->httpBodyOffset;
    }

    messageData->context.callback = messageData->context.pool
            .alloc<ClientHandlerCallback>(this, clientFd, messageData);
    engine.dispatchMessage(&messageData->context);
}

inline void writeHttpHeader(MemPool& pool, const char* name, const char* value)
{
    pool.putCString(name);
    pool.putData(": ", 2);
    pool.putCString(value);
    pool.putData("\r\n", 2);
}

void ClientHandler::processResponse(int clientFd, MessageData* messageData)
{
    auto it = clients.find(clientFd);
    if (it == clients.end()) {
        LogWarning() << "Failed to process response: non-existing client: " << clientFd;
        return;
    }

    ClientInfo* clientInfo = it->second;
    NGREST_ASSERT_NULL(clientInfo);

    clientInfo->requests.remove(messageData);
    messageData->poolStr.reset();

    // build response
    HttpResponse* response = static_cast<HttpResponse*>(messageData->context.response);
    messageData->poolStr.putData("HTTP/1.1 ", 9);
    if (response->statusCode == HTTP_STATUS_UNDEFINED)
        response->statusCode = HTTP_STATUS_200_OK;
    messageData->poolStr.putCString(HttpStatusInfo::httpStatusToString(
                                    static_cast<HttpStatus>(response->statusCode)));
    messageData->poolStr.putData("\r\n", 2);
    for (const Header* header = response->headers; header; header = header->next)
        writeHttpHeader(messageData->poolStr, header->name, header->value);

    // server
    writeHttpHeader(messageData->poolStr, "Server", "ngrest");

    // content-length
    uint64_t bodySize = response->poolBody.getSize();
    const int buffSize = 32;
    char buff[buffSize];
    NGREST_ASSERT(toCString(bodySize, buff, buffSize), "Failed to write Content-Length");
    writeHttpHeader(messageData->poolStr, "Content-Length", buff);

    // split body
    messageData->poolStr.putData("\r\n", 2);

    // write header to client
    const MemPool::Chunk* chunk = messageData->poolStr.getChunks();
    for (int i = 0, l = messageData->poolStr.getChunkCount(); i < l; ++i, ++chunk) {
        ::write(clientFd, chunk->buffer, chunk->size);
    }

    // write response body to client
    chunk = response->poolBody.getChunks();
    for (int i = 0, l = response->poolBody.getChunkCount(); i < l; ++i, ++chunk) {
        ::write(clientFd, chunk->buffer, chunk->size);
    }

    LogDebug() << "Request " << messageData->id << " handled in " << messageData->timer.elapsed() << " microsecond(s)";

    delete messageData;

    // delayed deleting ClientInfo after all requests are processed
    if (clientInfo->deleteLater && clientInfo->requests.empty())
        delete clientInfo;

    // response is to be freed by engine
}

void ClientHandler::processError(int clientFd, MessageData* messageData, const Exception& error)
{
    LogDebug() << "Error while handling request " << messageData->context.request->path;

    HttpResponse* response = static_cast<HttpResponse*>(messageData->context.response);
    if (response->statusCode == HTTP_STATUS_UNDEFINED) {
        try {
             throw; // we're called from catch block
        } catch (const HttpException& e) {
            response->statusCode = e.getHttpStatus();
        } catch (...) {
            response->statusCode = HTTP_STATUS_500_INTERNAL_SERVER_ERROR;
        }
    }
    Header headerContentType("Content-Type", "text/plain");
    response->headers = &headerContentType;
    response->poolBody.reset();
    response->poolBody.putCString(error.what());
    processResponse(clientFd, messageData);
}

}
