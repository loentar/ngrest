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
#include <ngrest/engine/HttpCommon.h>
#include <ngrest/engine/Engine.h>

#include "strutils.h"
#include "ClientHandler.h"

#define TRY_BLOCK_SIZE 512
#define MAX_REQUEST_SIZE 10485760 // 10 Mb

namespace ngrest {

struct Request
{
    MemPool poolStr;
    MemPool poolBin;
    bool isChunked = false;
    long contentLength = -1;
    uint64_t httpBodyOffset = 0;
    uint64_t httpBodyRemaining = -1;
//    uint64_t currentChunkRemaining = 0;
    HttpRequest httpRequest;
    bool processing = false;

    Request():
        poolStr(2048),
        poolBin(512)
    {
    }
};

struct ClientData
{
    char host[NI_MAXHOST];
    char port[NI_MAXSERV];

    std::list<Request*> requests;
};


class HttpResponseCallbackHandler: public HttpResponseCallback
{
public:
    HttpResponseCallbackHandler(ClientHandler* handler_, int clientFd_, Request* request_):
        handler(handler_), clientFd(clientFd_), request(request_)
    {
    }

    void onSuccess(const HttpResponse* response)
    {
        handler->processResponse(clientFd, request, response);
    }

    void onError(const Exception& error)
    {
        handler->processError(clientFd, request, error);
    }

    ClientHandler* handler;
    int clientFd;
    Request* request;
};


ClientHandler::ClientHandler(Engine& engine_):
    engine(engine_)
{
}

void ClientHandler::connected(int fd, const sockaddr* addr)
{
    ClientData*& client = clients[fd];
    if (client == nullptr) {
        client = new ClientData();

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
    delete clients[fd];
    clients.erase(fd);
}

void ClientHandler::error(int fd)
{
    LogError() << "Error client #" << fd;
}

bool ClientHandler::readyRead(int fd)
{
    ClientData* clientData = clients[fd];
    if (clientData == nullptr) {
        LogError() << "failed to find client #" << fd;
        return false;
    }

    Request* request;
    if (clientData->requests.empty()) {
        request = new Request();
        clientData->requests.push_back(request);
    } else {
        request = clientData->requests.back();
        NGREST_ASSERT_NULL(request);
        if (request->processing) {
            // request is finished to read and now processing.
            // creating new request
            request = new Request();
            clientData->requests.push_back(request);
        }
    }

    for (;;) {
        uint64_t sizeToRead = (request->httpBodyRemaining != -1)
                ? request->httpBodyRemaining : TRY_BLOCK_SIZE;
        char* buffer = request->poolStr.grow(sizeToRead);
        ssize_t count = ::read(fd, buffer, sizeToRead);
        if (count == 0) {
            // EOF. The remote has closed the connection.
            LogError() << "client #" << fd << " closed connection";
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
            request->poolStr.shrinkLastChunk(sizeToRead);
            break;
        }

        if (count < sizeToRead)
            request->poolStr.shrinkLastChunk(sizeToRead - count);

        if (request->httpBodyRemaining != -1)
            request->httpBodyRemaining -= count;

        if (request->httpBodyOffset == 0) {
            const char* startFind = ((buffer - 3) > request->poolStr.getLastChunk()->buffer)
                    ? (buffer - 3) : buffer;
            // will not be found if HTTP header size will be 4095-4097!
            const char* pos = strnstr(startFind, "\r\n\r\n", count);
            if (pos) {
                uint64_t httpHeaderSize = request->poolStr.getSize()
                        - request->poolStr.getLastChunk()->size + (pos - buffer);
                request->httpBodyOffset = httpHeaderSize + 4;

                MemPool::Chunk* chunk = request->poolStr.flatten();

                // parse HTTP header
                chunk->buffer[httpHeaderSize + 2] = '\0';
                chunk->buffer[httpHeaderSize + 3] = '\0';
                parseHttpHeader(chunk->buffer, request);

                const HttpHeader* headerEncoding = request->httpRequest.getHeader("transfer-encoding");
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
                    request->isChunked = true;
                } else {
                    const HttpHeader* headerLength = request->httpRequest.getHeader("content-length");
                    if (headerLength) {
                        NGREST_ASSERT(fromCString(headerLength->value, request->contentLength),
                                      "Failed to get content length of request");
                        NGREST_ASSERT(request->contentLength < MAX_REQUEST_SIZE,
                                      "Request is too large!");
                        const uint64_t totalRequestLength = request->httpBodyOffset +
                                request->contentLength;
                        request->httpBodyRemaining = totalRequestLength -
                                request->poolStr.getSize();
                        request->poolStr.reserve(totalRequestLength + 1);
                    }
                }

            }
        }

        if (request->httpBodyRemaining == 0) {
            request->httpRequest.clientHost = clientData->host;
            request->httpRequest.clientPort = clientData->port;
            processRequest(fd, request);
            break;
        }
    }

    return true;
}

void ClientHandler::parseHttpHeader(char* buffer, Request* client)
{
    char* curr = buffer;

    // parse method

    const char* method = token(curr);
    NGREST_ASSERT(method >= buffer, "Failed to get HTTP method");

    if (!strcmp(method, "POST")) {
        client->httpRequest.method = HTTP_METHOD_POST;
    } else if (!strcmp(method, "GET")) {
        client->httpRequest.method = HTTP_METHOD_GET;
    } else if (!strcmp(method, "PUT")) {
        client->httpRequest.method = HTTP_METHOD_PUT;
    } else if (!strcmp(method, "DELETE")) {
        client->httpRequest.method = HTTP_METHOD_DELETE;
    } else {
        client->httpRequest.method = HTTP_METHOD_UNKNOWN;
    }
    client->httpRequest.methodStr = method;


    // parse request url

    skipWs(curr);
    client->httpRequest.url = token(curr);
    NGREST_ASSERT(client->httpRequest.url > buffer, "Failed to get request URL");

    // seek to the first http header
    NGREST_ASSERT(seekTo(curr, '\n'), "Failed to seek to first HTTP header");
    skipWs(curr);


    // read http headers

    HttpHeader* lastHeader = nullptr;
    while (*curr != '\0') {
        char* name = token(curr, ':');
        NGREST_ASSERT(*curr, "Failed to parse HTTP header: unable to read name");
        trimRight(name, curr - 2);
        toLowerCase(name);
        skipWs(curr);
        NGREST_ASSERT(*curr, "Failed to parse HTTP header: unable to read value");
        char* value = token(curr, '\n');
        trimRight(value, curr - 2);
        HttpHeader* header = client->poolBin.alloc<HttpHeader>();
        header->name = name;
        header->value = value;
        if (lastHeader == nullptr) {
            client->httpRequest.headers = header;
        } else {
            lastHeader->next = header;
        }
        lastHeader = header;
    }
}

void ClientHandler::processRequest(int clientFd, Request* request)
{
    request->processing = true;
    MemPool::Chunk* chunk = request->poolStr.flatten();
    NGREST_ASSERT(request->httpBodyOffset <= chunk->size, "Request > size");
    request->httpRequest.body = chunk->buffer + request->httpBodyOffset;
    request->httpRequest.bodySize = chunk->size - request->httpBodyOffset;
//    const HttpHeader* headerContentType = client->request.getHeader("content-type");

    LogDebug() << "Request: " << request->httpRequest.methodStr << " " << request->httpRequest.url;
    LogDebug() << request->httpRequest.body;

    HttpResponseCallbackHandler* callback = new HttpResponseCallbackHandler(this, clientFd, request);
    engine.processRequest(&request->httpRequest, callback);
}

inline void writeHttpHeader(MemPool& pool, const char* name, const char* value)
{
    pool.putCString(name);
    pool.putData(": ", 2);
    pool.putCString(value);
    pool.putData("\r\n", 2);
}

void ClientHandler::processResponse(int clientFd, Request* request, const HttpResponse* response)
{
    auto it = clients.find(clientFd);
    if (it == clients.end()) {
        LogWarning() << "Failed to process response: non-existing client: " << clientFd;
        return;
    }

    ClientData* clientData = it->second;
    NGREST_ASSERT_NULL(clientData);

    clientData->requests.remove(request);
    request->poolStr.reset();

    // build response
    request->poolStr.putData("HTTP/1.1 ", 9);
    request->poolStr.putCString(HttpStatusInfo::httpStatusToString(
                                    static_cast<HttpStatus>(response->statusCode)));
    request->poolStr.putData("\r\n", 2);
    for (const HttpHeader* header = response->headers; header; header = header->next)
        writeHttpHeader(request->poolStr, header->name, header->value);

    // server
    writeHttpHeader(request->poolStr, "Server", "ngrest");

    // content-length
    uint64_t bodySize = response->poolBody.getSize();
    const int buffSize = 256;
    char buff[buffSize];
    NGREST_ASSERT(toCString(bodySize, buff, buffSize), "Failed to write Content-Length");
    writeHttpHeader(request->poolStr, "Content-Length", buff);

    // split body
    request->poolStr.putData("\r\n", 2);

    // write header to client
    const MemPool::Chunk* chunk = request->poolStr.getChunks();
    for (int i = 0, l = request->poolStr.getChunkCount(); i < l; ++i, ++chunk) {
        ::write(clientFd, chunk->buffer, chunk->size);
    }

    // write response body to client
    chunk = response->poolBody.getChunks();
    for (int i = 0, l = response->poolBody.getChunkCount(); i < l; ++i, ++chunk) {
        ::write(clientFd, chunk->buffer, chunk->size);
    }

    delete request;

    // response is to be freed by engine
}

void ClientHandler::processError(int clientFd, Request* request, const Exception& error)
{
    HttpResponse response;
    response.statusCode = HTTP_STATUS_500_INTERNAL_SERVER_ERROR;
    HttpHeader headerContentType("Content-Type", "text/plain");
    response.headers = &headerContentType;
    response.poolBody.reset();
    response.poolBody.putCString(error.what());
    processResponse(clientFd, request, &response);
}

}
