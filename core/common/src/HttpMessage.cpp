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

#include "HttpMessage.h"


namespace ngrest {

void HttpRequest::setMethod(const char* method)
{
    if (!strcmp(method, "POST")) {
        this->method = HttpMethod::POST;
    } else if (!strcmp(method, "GET")) {
        this->method = HttpMethod::GET;
    } else if (!strcmp(method, "PUT")) {
        this->method = HttpMethod::PUT;
    } else if (!strcmp(method, "DELETE")) {
        this->method = HttpMethod::DELETE;
    } else if (!strcmp(method, "PATCH")) {
        this->method = HttpMethod::PATCH;
    } else {
        this->method = HttpMethod::UNKNOWN;
    }
    methodStr = method;
}

}
