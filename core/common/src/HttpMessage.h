#ifndef NGREST_HTTPMESSAGE_H
#define NGREST_HTTPMESSAGE_H

#include "HttpMethod.h"
#include "HttpStatus.h"
#include "Message.h"

namespace ngrest {

struct HttpRequest: public Request
{
    HttpMethod method = HttpMethod::UNKNOWN;
    const char* methodStr = nullptr;

    const char* clientHost = nullptr;
    const char* clientPort = nullptr;
};


struct HttpResponse: public Response
{
    int statusCode = HTTP_STATUS_UNDEFINED;

//    // name must be in lower case
//    const Header* getHeader(const char* name) const;
};

}

#endif // NGREST_HTTPMESSAGE_H

