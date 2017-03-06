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

struct ClientContext
{
    Socket fd;
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];
    bool deleteLater = false;

    // due to http only one message can be processed at once per client
    // so we reuse just one message for all requests for this client

    ElapsedTimer timer;
    uint64_t id = 0;
    MessageContext context;
    HttpRequest request;
    HttpResponse response;
    MemPooler* pooler;
    MemPool* poolRead; // buffer to receive request
    MemPool* poolBody; // store request body and response headers
    MemPool* poolWrite; // response body
    bool usePoolBody = false;
    bool keepAliveConnection = true;
    uint64_t readOffset = 0;
    uint64_t currentRequestOffset = 0;
    uint64_t nextRequestOffset = INVALID_VALUE;
    uint64_t contentLength = INVALID_VALUE;
    uint64_t httpBodyOffset = 0;
    uint64_t httpBodyRemaining = INVALID_VALUE;
    bool processing = false;
    bool pipeline = false;
    bool needTryNext = false;
    uint8_t httpVersion = 0; // 0=unknown, 10 = 1.0, 11 = 1.1 ...

    // response data
    bool writing = false;
    MessageWriteState headerState;
    MessageWriteState bodyState;

    ClientContext(Socket fd_, Transport* transport, Engine* engine, MemPooler* pooler_):
        fd(fd_),
        pooler(pooler_),
        poolRead(pooler->obtain(2048)),
        poolBody(pooler->obtain(1024)),
        poolWrite(pooler->obtain(4096))
    {
        request.clientHost = host;
        request.clientPort = port;
        response.poolBody = poolWrite;

        context.pool = pooler->obtain();
        context.engine = engine;
        context.transport = transport;
        context.request = &request;
        context.response = &response;
    }

    ~ClientContext()
    {
        pooler->recycle(poolRead);
        pooler->recycle(poolBody);
        pooler->recycle(poolWrite);
        pooler->recycle(context.pool);
    }

    void reset()
    {
        // keep data in poolRead for the next request
        usePoolBody = false;
        contentLength = INVALID_VALUE;
        httpBodyOffset = 0;
        httpBodyRemaining = INVALID_VALUE;
        httpVersion = 0;
        writing = false;
        needTryNext = false;
        headerState = MessageWriteState();
        bodyState = MessageWriteState();

        poolBody->reset();
        poolWrite->reset();
        context.pool->reset();
        request = HttpRequest();
        request.clientHost = host;
        request.clientPort = port;
        response = HttpResponse();
        response.poolBody = poolWrite;
    }
};


class ClientHandlerCallback: public MessageCallback
{
public:
    ClientHandlerCallback(ClientHandler* handler_, ClientContext* clientContext_):
        handler(handler_), clientContext(clientContext_)
    {
    }

    void success()
    {
        handler->processResponse(clientContext);
    }

    void error(const Exception& error)
    {
        handler->processError(clientContext, error);
    }

    ClientHandler* handler;
    ClientContext* clientContext;
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
    ClientContext*& clientContext = clients[fd];
    if (clientContext == nullptr) {
        clientContext = new ClientContext(fd, &transport, &engine, pooler);

        int res = getnameinfo(reinterpret_cast<const sockaddr*>(addr), sizeof(*addr),
                              clientContext->host, sizeof(clientContext->host),
                              clientContext->port, sizeof(clientContext->port),
                              NI_NUMERICHOST | NI_NUMERICSERV);
        if (res == 0) {
            LogDebug() << "Accepted connection on client #" << fd
                       << " (host=" << clientContext->host << ", port=" << clientContext->port << ")";
        } else {
            LogWarning() << "Failed to get client info: " << gai_strerror(res);
            clientContext->host[0] = '\0';
            clientContext->port[0] = '\0';
        }
    } else {
        LogError() << "Client #" << fd << " is already connected";
    }
}

void ClientHandler::disconnected(Socket fd)
{
    auto it = clients.find(fd);
    if (it != clients.end()) {
        ClientContext* clientContext = clients[fd];
        if (!clientContext->processing && !clientContext->pipeline) {
            delete clientContext;
        } else {
            clientContext->deleteLater = true;
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

    ClientContext* clientContext = it->second;
    NGREST_ASSERT_NULL(clientContext);

    return readyRead(clientContext);
}

bool ClientHandler::readyRead(ClientContext* clientContext)
{
    for (;;) {
        MemPool* pool = clientContext->usePoolBody ? clientContext->poolBody : clientContext->poolRead;
        uint64_t prevSize = pool->getSize();
        uint64_t sizeToRead = (clientContext->httpBodyRemaining != INVALID_VALUE)
                ? clientContext->httpBodyRemaining : TRY_BLOCK_SIZE;
        char* buffer = pool->grow(sizeToRead);
        ssize_t received = ::recv(clientContext->fd, buffer, sizeToRead, 0);
        if (received == 0) {
            // this should only happen in case of ioctl(fd, FIONREAD...) failure
            // EOF. The remote has closed the connection.
            LogDebug() << "client #" << clientContext->fd << " closed connection";
            return false;
        }

        if (received == -1) {
            // some error
            if (errno != EAGAIN) {
                LogError() << "failed to read block from client #" << clientContext->fd
                           << ": " << strerror(errno);
                return false;
            }

            // all available data read
            pool->shrinkLastChunk(sizeToRead);
            break;
        }

        if (received < static_cast<int64_t>(sizeToRead))
            pool->shrinkLastChunk(sizeToRead - received);

        if (clientContext->httpBodyRemaining != INVALID_VALUE)
            clientContext->httpBodyRemaining -= received;

        if (clientContext->httpBodyOffset == 0) {
            pool->flatten();

            uint64_t findOffset = prevSize ? (prevSize - 3) : 0;
            Status res = tryParseHeaders(clientContext, pool, findOffset);
            switch (res) {
            case Status::Again:
                return true;
            case Status::Close:
                return false;
            default:;
            }
        }

        if (clientContext->httpBodyRemaining == 0) {
            try {
                processRequest(clientContext);
            } catch (const Exception& ex) {
                processError(clientContext, ex);
                return false; // close connection to client
            }
            break;
        }
    }

    return true;
}

Status ClientHandler::tryParseHeaders(ClientContext* clientContext, MemPool* pool, uint64_t findOffset)
{
    MemPool::Chunk* chunk = pool->getChunks();

    const char* pos = strstr(chunk->buffer + findOffset, "\r\n\r\n");
    if (pos) {
        uint64_t httpHeaderSize = pos - chunk->buffer - clientContext->currentRequestOffset;
        clientContext->httpBodyOffset = clientContext->currentRequestOffset + httpHeaderSize + 4;

        chunk->buffer[clientContext->httpBodyOffset - 2] = '\0'; // terminate HTTP header

        // parse HTTP header
        parseHttpHeader(chunk->buffer + clientContext->currentRequestOffset, clientContext);

        try {
            engine.runPhase(Phase::Header, &clientContext->context);
        } catch (const Exception& ex) {
            processError(clientContext, ex);
            return Status::Close; // close connection to client
        }

        Header* headerConnection = clientContext->request.getHeader("connection");
        if (headerConnection) {
            clientContext->keepAliveConnection = !strcasecmp(headerConnection->value, "keep-alive");
        } else {
            // HTTP 1.0 defaults connection to close
            // HTTP 1.1 defaults connection to keep-alive
            clientContext->keepAliveConnection = clientContext->httpVersion >= 11;
        }

        const Header* headerLength = clientContext->request.getHeader("content-length");
        if (headerLength) {
            NGREST_ASSERT(fromCString(headerLength->value, clientContext->contentLength), "Invalid value of content-length");
            NGREST_ASSERT(clientContext->contentLength < MAX_REQUEST_SIZE, "Request is too large!");
            const uint64_t totalRequestLength = clientContext->httpBodyOffset + clientContext->contentLength;
            clientContext->httpBodyRemaining = totalRequestLength - chunk->size;

            // if we didn't receive the whole body yet and chunk don't have enough space
            // store received part of body to another pool to avoid
            // Header* pointers damage on poolStr->flatten
            if (clientContext->httpBodyRemaining && (clientContext->httpBodyRemaining > (chunk->bufferSize - chunk->size))) {
                clientContext->poolBody->reserve(clientContext->contentLength + 1);
                // copy already received part of data to poolBody
                clientContext->poolBody->putData(chunk->buffer + clientContext->httpBodyOffset,
                                                 chunk->size - clientContext->httpBodyOffset);
                clientContext->usePoolBody = true;
                clientContext->nextRequestOffset = INVALID_VALUE; // no next request
            } else {
                clientContext->nextRequestOffset = totalRequestLength;
            }
        } else {
            Header* headerEncoding = clientContext->request.getHeader("transfer-encoding");
            if (headerEncoding) {
                if (!!strcasecmp(headerEncoding->value, "identity")) {
                    // FIXME: add support for transfer encodings
                    NGREST_THROW_ASSERT("This transfer-encoding is not supported:" + std::string(headerEncoding->value));
                }
            }
            // message has no body and ready to process now
            clientContext->httpBodyRemaining = 0;
            clientContext->nextRequestOffset = clientContext->httpBodyOffset;
        }

        return Status::Success;
    } else {
        return Status::Again;
    }
}

Status ClientHandler::readyWrite(Socket fd)
{
    auto it = clients.find(fd);
    if (it == clients.end()) {
        LogWarning() << "Failed to process request: non-existing client: " << fd;
        return Status::Close;
    }

    ClientContext* clientContext = it->second;
    NGREST_ASSERT_NULL(clientContext);

    if (!clientContext->writing)
        return Status::Success;

    // close connection if this is a last request and connection set to close
    bool closeConnection = !clientContext->keepAliveConnection;

    Status res = writeNextPart(clientContext);
    if (res == Status::Done && closeConnection)
        res = Status::Close;

    return res;
}

void ClientHandler::setCloseConnectionCallback(CloseConnectionCallback* callback)
{
    closeCallback = callback;
}

void ClientHandler::parseHttpHeader(char* buffer, ClientContext* clientContext)
{
    char* curr = buffer;

    // parse method

    const char* method = token(curr);
    NGREST_ASSERT(method >= buffer, "Failed to get HTTP method");
    HttpRequest* httpRequest = static_cast<HttpRequest*>(clientContext->context.request);
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
            clientContext->httpVersion = (httpVersionStr[0] - '0') * 10 + (httpVersionStr[2] - '0');
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
        Header* header = clientContext->context.pool->alloc<Header>();
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

void ClientHandler::processRequest(ClientContext* clientContext)
{
    clientContext->processing = true;
    clientContext->id = ++lastId;
    clientContext->timer.start();

    HttpRequest* httpRequest = static_cast<HttpRequest*>(clientContext->context.request);
    NGREST_ASSERT_NULL(httpRequest);

    if (clientContext->usePoolBody) {
        // handle body from poolBody with zero offset
        MemPool::Chunk* chunk = clientContext->poolBody->flatten();
        httpRequest->bodySize = chunk->size;
        httpRequest->body = chunk->buffer;
        httpRequest->poolBody = clientContext->poolBody;
    } else {
        if (clientContext->contentLength != INVALID_VALUE) {
            // handle body from poolStr with offset
            NGREST_ASSERT_NULL(clientContext->poolRead->getChunkCount() == 1); // should never happen

            const MemPool::Chunk* chunk = clientContext->poolRead->flatten(); // already flat, but we need NUL
            NGREST_ASSERT(clientContext->httpBodyOffset <= chunk->size, "Request > size");
            httpRequest->bodySize = chunk->size - clientContext->httpBodyOffset;
            if (httpRequest->bodySize)
                httpRequest->body = chunk->buffer + clientContext->httpBodyOffset;
        } else {
            httpRequest->bodySize = 0;
            httpRequest->body = nullptr;
        }
    }

    clientContext->context.callback = clientContext->context.pool
            ->alloc<ClientHandlerCallback>(this, clientContext);
    engine.dispatchMessage(&clientContext->context);
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

Status ClientHandler::tryNextRequest(ClientContext* clientContext)
{
    clientContext->reset();

    clientContext->currentRequestOffset = clientContext->nextRequestOffset;
    NGREST_ASSERT(clientContext->poolRead->getChunkCount() == 1, "Inconsistent mempool");
    MemPool::Chunk* chunk = clientContext->poolRead->getChunks();
    uint64_t remaining = chunk->size - clientContext->currentRequestOffset;
    if (!remaining) {
        // no data remaining - reset all
        clientContext->currentRequestOffset = 0;
        clientContext->nextRequestOffset = 0;
        clientContext->poolRead->reset();
        return Status::Again;
    }

    // some data remaining in the read buffer
    // there is a part of the next request or even the next full request

    Status status = tryParseHeaders(clientContext, clientContext->poolRead, clientContext->currentRequestOffset);
    switch (status) {
    case Status::Again:
        // only part of the next request, copy it to the beginning of mempool
        memmove(chunk->buffer, chunk->buffer + clientContext->currentRequestOffset, remaining);
        clientContext->currentRequestOffset = 0;
        clientContext->nextRequestOffset = 0;
        chunk->size = remaining;
        return Status::Again; // process to readyRead
    case Status::Close:
        return Status::Close;
    default:; // full request or at least full header
    }

    if (clientContext->httpBodyRemaining == 0) {
        // parse full request
        try {
            processRequest(clientContext);
            return Status::Success;
        } catch (const Exception& ex) {
            processError(clientContext, ex);
            return Status::Close;
        }
    }

    return Status::Again;
}

void ClientHandler::processResponse(ClientContext* clientContext)
{
    clientContext->writing = true;
    clientContext->poolBody->reset();

    // build response
    HttpResponse* response = static_cast<HttpResponse*>(clientContext->context.response);
    clientContext->poolBody->putData(clientContext->httpVersion == 10 ? "HTTP/1.0 " : "HTTP/1.1 ", 9);
    if (response->statusCode == HTTP_STATUS_UNDEFINED)
        response->statusCode = HTTP_STATUS_200_OK;
    clientContext->poolBody->putCString(HttpStatusInfo::httpStatusToString(static_cast<HttpStatus>(response->statusCode)));
    clientContext->poolBody->putData("\r\n", 2);
    for (const Header* header = response->headers; header; header = header->next)
        writeHttpHeader(clientContext->poolBody, header->name, header->value);

    // server
    writeHttpHeader(clientContext->poolBody, "Server", "ngrest");

    // content-length
    uint64_t bodySize = response->poolBody->getSize();
    const int buffSize = 32;
    char buff[buffSize];
    NGREST_ASSERT(toCString(bodySize, buff, buffSize), "Failed to write Content-Length");
    writeHttpHeader(clientContext->poolBody, "Content-Length", buff);
    const char* serverDate = getServerDate();
    if (serverDate)
        writeHttpHeader(clientContext->poolBody, "Date", serverDate);
    writeHttpHeader(clientContext->poolBody, "Connection", clientContext->keepAliveConnection ? "Keep-Alive" : "Close");

    // split body
    clientContext->poolBody->putData("\r\n", 2);

    clientContext->headerState.chunk = clientContext->poolBody->getChunks();
    clientContext->headerState.end = clientContext->poolBody->getLastChunk() + 1;
    clientContext->headerState.pos = 0;
    if (!response->poolBody->isClean()) {
        clientContext->bodyState.chunk = response->poolBody->getChunks();
        clientContext->bodyState.end = response->poolBody->getLastChunk() + 1;
        clientContext->bodyState.pos = 0;
    }

    bool closeConnection = !clientContext->keepAliveConnection;

    Status res = writeNextPart(clientContext);
    // clientContext and clientContext is freed here when res=Done
    if (res == Status::Close || (res == Status::Done && closeConnection)) {
        NGREST_ASSERT_NULL(closeCallback);
        LogDebug() << "Closing connection to client";
        closeCallback->closeConnection(clientContext->fd);
    }
}

void ClientHandler::processError(ClientContext* clientContext, const Exception& error)
{
    LogDebug() << "Error while handling request " << clientContext->context.request->path;

    HttpResponse* response = static_cast<HttpResponse*>(clientContext->context.response);
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
    processResponse(clientContext);
}

inline Status writeChunks(Socket fd, MessageWriteState& state)
{
    while (state.chunk != state.end) {
        ssize_t sent = ::send(fd, state.chunk->buffer + state.pos, state.chunk->size - state.pos, 0);
        if (sent == -1) {
            // output buffer is full.
            if (errno == EAGAIN)
                return Status::Again;

            // other error
            if (errno != EPIPE && errno != ECONNRESET)
                LogError() << "Failed to write response: " << Error::getLastError();
            return Status::Close;
        }

        state.pos += static_cast<uint64_t>(sent);
        if (state.pos != state.chunk->size)
            continue;

        state.pos = 0;
        ++state.chunk;
    }

    return Status::Success;
}

Status ClientHandler::writeNextPart(ClientContext* clientContext)
{
    // write header to client
    Status writeStatus = writeChunks(clientContext->fd, clientContext->headerState);
    if (writeStatus == Status::Again)
        return writeStatus;

    if (writeStatus == Status::Success) {
        if (clientContext->bodyState.chunk) {
            // write response body to client
            writeStatus = writeChunks(clientContext->fd, clientContext->bodyState);
            if (writeStatus == Status::Again)
                return writeStatus;
        }
    }

    LogDebug() << "Request " << clientContext->id << " handled in "
               << clientContext->timer.elapsed() << " microsecond(s)";
    clientContext->processing = false;

    Status res = Status::Done;

    if (clientContext->pipeline) {
        clientContext->needTryNext = true;
        return res;
    }

    // delayed deletion of ClientContext if connection was closed meanwhile processing
    if (clientContext->deleteLater) {
        delete clientContext;
        return res;
    }

    clientContext->pipeline = true;
    do {
        Status tryRes = Status::Done;
        do {
            tryRes = tryNextRequest(clientContext);
            if (tryRes == Status::Again)
                break;
            if (tryRes == Status::Close) {
                res = Status::Close;
                clientContext->needTryNext = false;
            }
        } while (clientContext->needTryNext);

        if (tryRes == Status::Again) {
            clientContext->nextRequestOffset = INVALID_VALUE;
            // request with body read, try to continue reqding the next request
            // we don't get event for it in edge trigger mode

            if (!readyRead(clientContext)) {
                clientContext->needTryNext = false;
                res = Status::Close;
            }
        }
    } while (clientContext->needTryNext);
    clientContext->pipeline = false;

    return res;
}

}
