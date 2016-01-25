#include <string.h>

#include <ngrest/utils/Log.h>
#include <ngrest/utils/MemPool.h>
#include <ngrest/utils/Exception.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/json/JsonReader.h>
#include <ngrest/json/JsonWriter.h>

#include "HttpTransport.h"

namespace ngrest {

HttpTransport::HttpTransport():
    Transport(Type::Http)
{
}

Node* HttpTransport::parseRequest(MemPool& pool, const Request* request)
{
    const HttpRequest* httpRequest = static_cast<const HttpRequest*>(request);
    LogDebug() << "HTTP Request: " << httpRequest->methodStr << " " << httpRequest->path;
    LogDebug() << "-- body -------------\n" << httpRequest->body << "\n-----------------\n";

    const Header* contentType = httpRequest->getHeader("content-type");
    NGREST_ASSERT(contentType, "Content-Type header is missing!");

    if (!strcmp(contentType->value, "application/json")) {
        // JSON request
        return json::JsonReader::read(httpRequest->body, pool);
    } else {
        NGREST_THROW_ASSERT(std::string("Can't handle content type: ") + contentType->value);
    }
}

void HttpTransport::writeResponse(MemPool& pool, const Request* request, Response* response)
{
    const HttpRequest* httpRequest = static_cast<const HttpRequest*>(request);
    const Header* contentType = httpRequest->getHeader("content-type");

    // response with JSON if no "content-type" header set
    if (!contentType || !strcmp(contentType->value, "application/json")) {
        HttpResponse* httpResponse = static_cast<HttpResponse*>(response);
        Header* headerContentType = pool.alloc<Header>("Content-Type", "application/json");
        httpResponse->headers = headerContentType;

        NGREST_ASSERT_NULL(httpResponse->node);
        return json::JsonWriter::write(httpResponse->node, httpResponse->poolBody);
    } else {
        NGREST_THROW_ASSERT(std::string("Can't handle content type: ") + contentType->value);
    }
}

int HttpTransport::getRequestMethod(const Request* request)
{
    return static_cast<int>(static_cast<const HttpRequest*>(request)->method);
}

} // namespace ngrest


