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
#include <ngrest/utils/static.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/json/JsonReader.h>
#include <ngrest/json/JsonWriter.h>

#include "HttpTransport.h"

namespace ngrest {

#define CONTENT_TYPE_APPLICATION_JSON "application/json"
#define CONTENT_TYPE_APPLICATION_JSON_LEN static_strlen(CONTENT_TYPE_APPLICATION_JSON)


inline const char* strchrnul(const char* str, const char ch)
{
    NGREST_ASSERT_NULL(str);
    while (*str && (*str != ch))
        ++str;
    return str;
}

inline void trim(const char*& begin, const char*& end)
{
    while (begin < end && *begin == ' ')
        ++begin;
    while (begin < end && *(end - 1) == ' ')
        --end;
}

HttpTransport::HttpTransport():
    Transport(Type::Http)
{
}

Node* HttpTransport::parseRequest(MemPool* pool, Request* request)
{
    HttpRequest* httpRequest = static_cast<HttpRequest*>(request);
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

    const char* begin = contentType->value;
    const char* end = strchrnul(begin, ';'); // application/json;charset=utf-8
    trim(begin, end);

    if ((end - begin) == CONTENT_TYPE_APPLICATION_JSON_LEN
            && !strncmp(begin, CONTENT_TYPE_APPLICATION_JSON, CONTENT_TYPE_APPLICATION_JSON_LEN)) {
        httpRequest->contentType = ContentType::ApplicationJson;
    } else {
        httpRequest->contentType = ContentType::Unknown;
    }

    switch (httpRequest->contentType) {
    case ContentType::ApplicationJson: // JSON request
        return json::JsonReader::read(httpRequest->body, pool);

    default:
        NGREST_THROW_ASSERT(std::string("Can't handle content type: ") + contentType->value);
    }
}

void HttpTransport::writeResponse(MemPool* pool, const Request* request, Response* response)
{
    const HttpRequest* httpRequest = static_cast<const HttpRequest*>(request);

    // respond with JSON if no "content-type" header set
    switch (httpRequest->contentType) {
    case ContentType::NotSet:
    case ContentType::ApplicationJson: {
        HttpResponse* httpResponse = static_cast<HttpResponse*>(response);
        response->headers = pool->alloc<Header>("Content-Type", "application/json", response->headers);

        if (httpResponse->node)
            json::JsonWriter::write(httpResponse->node, httpResponse->poolBody);

        break;
    }

    default: {
        NGREST_THROW_ASSERT("Can't write response: unsupported content type");
    }
    }
}

int HttpTransport::getRequestMethod(const Request* request)
{
    return static_cast<int>(static_cast<const HttpRequest*>(request)->method);
}

} // namespace ngrest


