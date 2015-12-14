#ifndef NGREST_SERVERCOMMON_H
#define NGREST_SERVERCOMMON_H

#include <stdint.h>
#include <ngrest/utils/MemPool.h>

#include "HttpStatus.h"

namespace ngrest {

enum HttpMethod
{
    HTTP_METHOD_UNKNOWN,
    HTTP_METHOD_POST,
    HTTP_METHOD_GET,
    HTTP_METHOD_PUT,
    HTTP_METHOD_DELETE
};

struct HttpHeader
{
    const char* name;
    const char* value;
    const HttpHeader* next;

    HttpHeader(const char* name_ = nullptr, const char* value_ = nullptr, const HttpHeader* next_ = nullptr):
        name(name_), value(value_), next(next_)
    {
    }
};

struct HttpRequest
{
    HttpMethod method = HTTP_METHOD_UNKNOWN;
    const char* methodStr = nullptr;
    const char* url = nullptr;
    const HttpHeader* headers = nullptr;
    char* body = nullptr;
    uint64_t bodySize = 0;

    const char* clientHost = nullptr;
    const char* clientPort = nullptr;

    // name must be in lower case
    const HttpHeader* getHeader(const char* name) const;
};


struct HttpResponse
{
    int statusCode = HTTP_STATUS_200_OK;
    const HttpHeader* headers = nullptr;
    MemPool poolBody;
//    char* body = nullptr;
//    uint64_t bodySize = 0;

//    // name must be in lower case
//    const HttpHeader* getHeader(const char* name) const;
};

}

#endif // NGREST_SERVERCOMMON_H

