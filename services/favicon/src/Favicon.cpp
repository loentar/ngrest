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

#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/HttpException.h>
#include <ngrest/engine/Transport.h>

#include "Favicon.h"
#include "favicon_ico.h"

namespace ngrest {

void Favicon::favicon(MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    Header* headerContentType = context.pool->alloc<Header>("Content-Type", "image/x-icon");
    response->headers = headerContentType;

    context.response->poolBody->putData(reinterpret_cast<const char*>(favicon_ico), favicon_ico_len);
}

}
