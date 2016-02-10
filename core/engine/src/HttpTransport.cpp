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

#include <string.h>
#include <stdlib.h>

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
#ifdef DEBUG
    static const bool dontTruncate = !!getenv("NGREST_DONT_TRUNCATE_REQUEST");
    if (dontTruncate || httpRequest->bodySize < 1024) {
        LogDebug() << "-- body -------------\n" << httpRequest->body << "\n-----------------\n";
    } else {
        (LogDebug() << "-- body -------------\n").write(httpRequest->body, 1024)
                    << "... (truncated)\n-----------------\n";
    }
#endif

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

        if (httpResponse->node)
            json::JsonWriter::write(httpResponse->node, httpResponse->poolBody);
    } else {
        NGREST_THROW_ASSERT(std::string("Can't handle content type: ") + contentType->value);
    }
}

int HttpTransport::getRequestMethod(const Request* request)
{
    return static_cast<int>(static_cast<const HttpRequest*>(request)->method);
}

} // namespace ngrest


