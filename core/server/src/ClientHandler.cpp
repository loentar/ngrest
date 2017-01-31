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
#include <string.h>
#ifndef WIN32
#ifndef __APPLE__
#include <error.h>
#endif
#include <sys/socket.h>
#include <netdb.h>
#else
#include <Ws2tcpip.h>
#undef DELETE // conflicts with HttpMethod::DELETE
#endif
#include <time.h>

#include <list>

#include <ngrest/utils/Log.h>
#include <ngrest/utils/MemPool.h>
#include <ngrest/utils/MemPooler.h>
#include <ngrest/utils/Exception.h>
#include <ngrest/utils/fromcstring.h>
#include <ngrest/utils/tocstring.h>
#include <ngrest/utils/ElapsedTimer.h>
#include <ngrest/utils/Error.h>
#include <ngrest/common/Message.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/HttpException.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/Phase.h>

#include "strutils.h"
#include "ClientHandler.h"

#define TRY_BLOCK_SIZE 512
#define MAX_REQUEST_SIZE 10485760 // 10 Mb

namespace ngrest {

static const uint64_t INVALID_VALUE = static_cast<uint64_t>(-1);

struct MessageWriteState
{
    const MemPool::Chunk* chunk = nullptr;
    const MemPool::Chunk* end = nullptr;
    uint64_t pos = 0;
};

struct MessageData
{
    ElapsedTimer timer;
    uint64_t id;
    MessageContext context;
    MemPooler* pooler;
    MemPool* poolStr;
    MemPool* poolBody;
    bool usePoolBody = false;
    bool keepAliveConnection = true;
    uint64_t contentLength = INVALID_VALUE;
    uint64_t httpBodyOffset = 0;
    uint64_t httpBodyRemaining = INVALID_VALUE;
    bool processing = false;
    uint8_t httpVersion = 0; // 0=unknown, 10 = 1.0, 11 = 1.1 ...

    // response data
    bool writing = false;
    MessageWriteState headerState;
    MessageWriteState bodyState;

    MessageData(uint64_t id_, Transport* transport, Engine* engine, MemPooler* pooler_):
        timer(true),
        id(id_),
        pooler(pooler_),
        poolStr(pooler_->obtain(2048)),
        poolBody(pooler_->obtain(1024))
    {
        LogDebug() << "Start handling request " << id;
        context.pool = pooler->obtain();
        context.engine = engine;
        context.transport = transport;
        context.request = context.pool->alloc<HttpRequest>();
        context.response = context.pool->alloc<HttpResponse>();
        context.response->poolBody = pooler->obtain(65536); // 64 KB chunks for output buffer
    }

    ~MessageData()
    {
        pooler->recycle(poolStr);
        pooler->recycle(poolBody);
        pooler->recycle(context.response->poolBody);
        pooler->recycle(context.pool);
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
    ClientHandlerCallback(ClientHandler* handler_, Socket clientFd_, MessageData* messageData_):
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
    Socket clientFd;
    MessageData* messageData;
};


ClientHandler::ClientHandler(Engine& engine_, Transport& transport_):
    engine(engine_), transport(transport_), pooler(new MemPooler())
{
}

ClientHandler::~ClientHandler()
{
    delete pooler;
}

void ClientHandler::connected(Socket fd, const sockaddr_storage* addr)
{
    ClientInfo*& client = clients[fd];
    if (client == nullptr) {
        client = new ClientInfo();

        int res = getnameinfo(reinterpret_cast<const sockaddr*>(addr), sizeof(*addr),
                              client->host, sizeof(client->host),
                              client->port, sizeof(client->port),
                              NI_NUMERICHOST | NI_NUMERICSERV);
        if (res == 0) {
            LogDebug() << "Accepted connection on client #" << fd
                       << " (host=" << client->host << ", port=" << client->port << ")";
        } else {
            LogWarning() << "Failed to get client info: " << gai_strerror(res);
            client->host[0] = '\0';
            client->port[0] = '\0';
        }
    } else {
        LogError() << "Client #" << fd << " is already connected";
    }
}

void ClientHandler::disconnected(Socket fd)
{
    auto it = clients.find(fd);
    if (it != clients.end()) {
        ClientInfo* clientInfo = clients[fd];
        if (clientInfo->requests.empty()) {
            delete clientInfo;
        } else {
            clientInfo->deleteLater = true;
        }
        clients.erase(fd);
    }
    LogDebug() << "client #" << fd << " disconnected";
}

void ClientHandler::error(Socket fd)
{
//    LogError() << "Error client #" << fd;
}

bool ClientHandler::readyRead(Socket fd)
{
    auto it = clients.find(fd);
    if (it == clients.end()) {
        LogWarning() << "Failed to process request: non-existing client: " << fd;
        return false;
    }

    ClientInfo* clientData = it->second;
    NGREST_ASSERT_NULL(clientData);

    MessageData* messageData;

    if (clientData->requests.empty()) {
        messageData = new MessageData(++lastId, &transport, &engine, pooler);
        clientData->requests.push_back(messageData);
    } else {
        messageData = clientData->requests.back();
        NGREST_ASSERT_NULL(messageData);

        if (messageData->processing) {
            // request is finished to read and now processing.
            // creating new request
            messageData = new MessageData(++lastId, &transport, &engine, pooler);
            clientData->requests.push_back(messageData);
        }
    }

    HttpRequest* httpRequest = static_cast<HttpRequest*>(messageData->context.request);
    for (;;) {
        MemPool* pool = messageData->usePoolBody ? messageData->poolBody : messageData->poolStr;
        uint64_t prevSize = pool->getSize();
        uint64_t sizeToRead = (messageData->httpBodyRemaining != INVALID_VALUE)
                ? messageData->httpBodyRemaining : TRY_BLOCK_SIZE;
        char* buffer = pool->grow(sizeToRead);
        ssize_t count = ::recv(fd, buffer, sizeToRead, 0);
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
            pool->shrinkLastChunk(sizeToRead);
            break;
        }

        if (count < static_cast<int64_t>(sizeToRead))
            pool->shrinkLastChunk(sizeToRead - count);

        if (messageData->httpBodyRemaining != INVALID_VALUE)
            messageData->httpBodyRemaining -= count;

        if (messageData->httpBodyOffset == 0) {
            MemPool::Chunk* chunk = pool->flatten(false);
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

            const char* pos = strnstrn(startFind, "\r\n\r\n", searchSize, 4);
            if (pos) {
                uint64_t httpHeaderSize = prevSize + (pos - buffer);
                messageData->httpBodyOffset = httpHeaderSize + 4;

                chunk->buffer[httpHeaderSize + 2] = '\0'; // terminate HTTP header

                // parse HTTP header
                parseHttpHeader(chunk->buffer, messageData);

                try {
                    engine.runPhase(Phase::Header, &messageData->context);
                } catch (const Exception& ex) {
                    processError(fd, messageData, ex);
                    return false; // close connection to client
                }

                Header* headerConnection = httpRequest->getHeader("connection");
                if (headerConnection) {
                    messageData->keepAliveConnection = !strcasecmp(headerConnection->value, "keep-alive");
                } else {
                    // HTTP 1.0 defaults connection to close
                    // HTTP 1.1 defaults connection to keep-alive
                    messageData->keepAliveConnection = messageData->httpVersion >= 11;
                }

                const Header* headerLength = httpRequest->getHeader("content-length");
                if (headerLength) {
                    NGREST_ASSERT(fromCString(headerLength->value, messageData->contentLength),
                                  "Failed to get content length of request");
                    NGREST_ASSERT(messageData->contentLength < MAX_REQUEST_SIZE,
                                  "Request is too large!");
                    const uint64_t totalRequestLength = messageData->httpBodyOffset +
                            messageData->contentLength;
                    messageData->httpBodyRemaining = totalRequestLength - pool->getSize();
                    messageData->poolBody->reserve(totalRequestLength + 1);

                    // if we didn't receive the whole body yet
                    // store received part of body to another pool to avoid
                    // Header* pointers damage on poolStr->flatten
                    if (messageData->httpBodyRemaining) {
                        // copy already received part of data to poolBody
                        messageData->poolBody->putData(chunk->buffer + messageData->httpBodyOffset,
                                                      chunk->size - messageData->httpBodyOffset);
                        messageData->usePoolBody = true;
                    }
                } else {
                    Header* headerEncoding = httpRequest->getHeader("transfer-encoding");
                    if (headerEncoding) {
                        if (!!strcasecmp(headerEncoding->value, "identity")) {
                            // FIXME: add support for transfer encodings
                            NGREST_THROW_ASSERT("This transfer-encoding is not supported:" + std::string(headerEncoding->value));
                        }
                    }
                    // message has no body and ready to process now
                    messageData->httpBodyRemaining = 0;
                }
            }
        }

        if (messageData->httpBodyRemaining == 0) {
            httpRequest->clientHost = clientData->host;
            httpRequest->clientPort = clientData->port;
            try {
                processRequest(fd, messageData);
            } catch (const Exception& ex) {
                processError(fd, messageData, ex);
                return false; // close connection to client
            }
            break;
        }
    }

    return true;
}

WriteStatus ClientHandler::readyWrite(Socket fd)
{
    auto it = clients.find(fd);
    if (it == clients.end()) {
        LogWarning() << "Failed to process request: non-existing client: " << fd;
        return WriteStatus::Close;
    }

    ClientInfo* clientData = it->second;
    NGREST_ASSERT_NULL(clientData);

    // nothing to write: all requests already finished
    if (clientData->requests.empty())
        return WriteStatus::Success;

    MessageData* messageData = nullptr;
    // find first unfinished write
    for (MessageData* message : clientData->requests) {
        if (message->writing) {
            messageData = message;
            break;
        }
    }

    // nothing to write: no requests to write but some to read
    if (!messageData)
        return WriteStatus::Success;

    // close connection if this is a last request and connection set to close
    bool closeConnection = !messageData->keepAliveConnection &&
            clientData->requests.size() == 1;

    WriteStatus res = writeNextPart(fd, clientData, messageData);
    // clientData and messageData is freed here when res=Done
    if (res == WriteStatus::Done && closeConnection)
        res = WriteStatus::Close;

    return res;
}

void ClientHandler::setCloseConnectionCallback(CloseConnectionCallback* callback)
{
    closeCallback = callback;
}

void ClientHandler::parseHttpHeader(char* buffer, MessageData* messageData)
{
    char* curr = buffer;

    // parse method

    const char* method = token(curr);
    NGREST_ASSERT(method >= buffer, "Failed to get HTTP method");
    HttpRequest* httpRequest = static_cast<HttpRequest*>(messageData->context.request);
    NGREST_ASSERT_NULL(httpRequest);

    httpRequest->setMethod(method);

    // parse request url

    skipWs(curr);
    httpRequest->path = token(curr);
    NGREST_ASSERT(httpRequest->path > buffer, "Failed to get request URL");

    skipWs(curr);
    char* httpVersionStr = curr;

    // seek to the first http header
    NGREST_ASSERT(seekTo(curr, '\n'), "Failed to seek to first HTTP header");

    // parse http version;
    if (!strncmp(httpVersionStr, "HTTP/", 5)) {
        httpVersionStr += 5;
        int len = curr - httpVersionStr;
        if ((len == 4 || len == 3) && httpVersionStr[1] == '.') { // "1.0\r"
            messageData->httpVersion = (httpVersionStr[0] - '0') * 10 + (httpVersionStr[2] - '0');
        }
    }

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
        Header* header = messageData->context.pool->alloc<Header>();
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

void ClientHandler::processRequest(Socket clientFd, MessageData* messageData)
{
    messageData->processing = true;

    HttpRequest* httpRequest = static_cast<HttpRequest*>(messageData->context.request);
    NGREST_ASSERT_NULL(httpRequest);

    if (messageData->usePoolBody) {
        // handle body from poolBody with zero offset
        MemPool::Chunk* chunk = messageData->poolBody->flatten();
        httpRequest->bodySize = chunk->size;
        httpRequest->body = chunk->buffer;
        httpRequest->poolBody = messageData->poolBody;
    } else {
        // handle body from poolStr with offset
        NGREST_ASSERT_NULL(messageData->poolStr->getChunkCount() == 1); // should never happen

        const MemPool::Chunk* chunk = messageData->poolStr->flatten(); // already flat, but we need NUL
        NGREST_ASSERT(messageData->httpBodyOffset <= chunk->size, "Request > size");
        httpRequest->bodySize = chunk->size - messageData->httpBodyOffset;
        if (httpRequest->bodySize)
            httpRequest->body = chunk->buffer + messageData->httpBodyOffset;
    }

    messageData->context.callback = messageData->context.pool
            ->alloc<ClientHandlerCallback>(this, clientFd, messageData);
    engine.dispatchMessage(&messageData->context);
}

inline void writeHttpHeader(MemPool* pool, const char* name, const char* value)
{
    pool->putCString(name);
    pool->putData(": ", 2);
    pool->putCString(value);
    pool->putData("\r\n", 2);
}

const char* ClientHandler::getServerDate()
{
#ifdef WIN32
    SYSTEMTIME date;
    GetSystemTime(&date);

    if (date.wSecond != lastDate.wSecond ||
            date.wMinute != lastDate.wMinute ||
            date.wHour != lastDate.wHour ||
            date.wDay != lastDate.wDay ||
            date.wMonth != lastDate.wMonth ||
            date.wYear != lastDate.wYear ||
            date.wDayOfWeek != lastDate.wDayOfWeek) {

        struct tm gmt;

        gmt.tm_sec = date.wSecond;
        gmt.tm_min = date.wMinute;
        gmt.tm_hour = date.wHour;
        gmt.tm_mday = date.wDay;
        gmt.tm_mon = date.wMonth - 1;
        gmt.tm_year = date.wYear - 1900;
        gmt.tm_wday = date.wDayOfWeek;
        gmt.tm_yday = 0;
        gmt.tm_isdst = 0;
#else
    time_t date;
    time(&date);
    if (date != lastDate) {
        struct tm gmt;
        if (!gmtime_r(&date, &gmt))
            return nullptr;
#endif

        if (strftime(dateBuff, dateBuffSize, "%a, %b %d %Y %H:%M:%S GMT", &gmt) >= dateBuffSize)
            return nullptr;

        lastDate = date;
    }

    return dateBuff;
}

void ClientHandler::processResponse(Socket clientFd, MessageData* messageData)
{
    auto it = clients.find(clientFd);
    if (it == clients.end()) {
        LogWarning() << "Failed to process response: non-existing client: " << clientFd;
        return;
    }

    ClientInfo* clientInfo = it->second;
    NGREST_ASSERT_NULL(clientInfo);

    messageData->writing = true;

    messageData->poolStr->reset();

    // build response
    HttpResponse* response = static_cast<HttpResponse*>(messageData->context.response);
    messageData->poolStr->putData(messageData->httpVersion == 10 ? "HTTP/1.0 " : "HTTP/1.1 ", 9);
    if (response->statusCode == HTTP_STATUS_UNDEFINED)
        response->statusCode = HTTP_STATUS_200_OK;
    messageData->poolStr->putCString(HttpStatusInfo::httpStatusToString(
                                    static_cast<HttpStatus>(response->statusCode)));
    messageData->poolStr->putData("\r\n", 2);
    for (const Header* header = response->headers; header; header = header->next)
        writeHttpHeader(messageData->poolStr, header->name, header->value);

    // server
    writeHttpHeader(messageData->poolStr, "Server", "ngrest");

    // content-length
    uint64_t bodySize = response->poolBody->getSize();
    const int buffSize = 32;
    char buff[buffSize];
    NGREST_ASSERT(toCString(bodySize, buff, buffSize), "Failed to write Content-Length");
    writeHttpHeader(messageData->poolStr, "Content-Length", buff);
    const char* serverDate = getServerDate();
    if (serverDate)
        writeHttpHeader(messageData->poolStr, "Date", serverDate);
    writeHttpHeader(messageData->poolStr, "Connection", messageData->keepAliveConnection ? "Keep-Alive" : "Close");

    // split body
    messageData->poolStr->putData("\r\n", 2);

    messageData->headerState.chunk = messageData->poolStr->getChunks();
    messageData->headerState.end = messageData->poolStr->getLastChunk() + 1;
    messageData->headerState.pos = 0;
    if (!response->poolBody->isClean()) {
        messageData->bodyState.chunk = response->poolBody->getChunks();
        messageData->bodyState.end = response->poolBody->getLastChunk() + 1;
        messageData->bodyState.pos = 0;
    }

    bool closeConnection = !messageData->keepAliveConnection &&
            clientInfo->requests.size() == 1;

    WriteStatus res = writeNextPart(clientFd, clientInfo, messageData);
    // clientData and messageData is freed here when res=Done
    if (res == WriteStatus::Done && closeConnection) {
        NGREST_ASSERT_NULL(closeCallback);
        LogDebug() << "Closing connection to client";
        closeCallback->closeConnection(clientFd);
    }
}

void ClientHandler::processError(Socket clientFd, MessageData* messageData, const Exception& error)
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
    response->poolBody->reset();
    response->poolBody->putCString(error.what());
    processResponse(clientFd, messageData);
}

inline WriteStatus writeChunks(Socket fd, MessageWriteState& state)
{
    while (state.chunk != state.end) {
        ssize_t sent = ::send(fd, state.chunk->buffer + state.pos, state.chunk->size - state.pos, 0);
        if (sent == -1) {
            // output buffer is full.
            if (errno == EAGAIN)
                return WriteStatus::Again;

            // other error
            if (errno != EPIPE)
                LogError() << "Failed to write response: " << Error::getLastError();
            return WriteStatus::Close;
        }

        state.pos += static_cast<uint64_t>(sent);
        if (state.pos != state.chunk->size)
            continue;

        state.pos = 0;
        ++state.chunk;
    }

    return WriteStatus::Success;
}

WriteStatus ClientHandler::writeNextPart(Socket clientFd, ClientInfo* clientInfo, MessageData* messageData)
{
    // write header to client
    WriteStatus writeStatus = writeChunks(clientFd, messageData->headerState);

    if (writeStatus == WriteStatus::Again)
        return writeStatus;

    if (writeStatus == WriteStatus::Success) {
        if (messageData->bodyState.chunk) {
            // write response body to client
            writeStatus = writeChunks(clientFd, messageData->bodyState);
            if (writeStatus == WriteStatus::Again)
                return writeStatus;
        }
    }

    LogDebug() << "Request " << messageData->id << " handled in "
               << messageData->timer.elapsed() << " microsecond(s)";

    clientInfo->requests.remove(messageData);

    delete messageData;

    // delayed deleting ClientInfo after all requests are processed
    if (clientInfo->deleteLater && clientInfo->requests.empty())
        delete clientInfo;

    // response is to be freed by engine
    return WriteStatus::Done;
}

}
