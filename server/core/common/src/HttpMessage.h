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

#ifndef NGREST_HTTPMESSAGE_H
#define NGREST_HTTPMESSAGE_H

#include "HttpMethod.h"
#include "HttpStatus.h"
#include "Message.h"

namespace ngrest {

enum class ContentType {
    Unknown,
    NotSet,
    ApplicationJson
};

/**
 * @brief HTTP request
 */
struct NGREST_COMMON_EXPORT HttpRequest: public Request
{
    HttpMethod method = HttpMethod::UNKNOWN;   //!< parsed HTTP method
    const char* methodStr = nullptr;           //!< HTTP method as given by client

    const char* clientHost = nullptr;          //!< client host address
    const char* clientPort = nullptr;          //!< client port

    ContentType contentType = ContentType::NotSet; //!< request content type

    void setMethod(const char* method);
};


/**
 * @brief HTTP response
 */
struct NGREST_COMMON_EXPORT HttpResponse: public Response
{
    int statusCode = HTTP_STATUS_UNDEFINED;     //! http status code of response
};

}

#endif // NGREST_HTTPMESSAGE_H

