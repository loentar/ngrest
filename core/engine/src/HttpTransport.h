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

#ifndef NGREST_HTTPTRANSPORT_H
#define NGREST_HTTPTRANSPORT_H

#include "Transport.h"

namespace ngrest {

class HttpTransport: public Transport
{
public:
    HttpTransport();

    // pool temporary pool for storing data
    virtual Node* parseRequest(MemPool* pool, const Request* request) override;

    // pool temporary pool for storing data
    virtual void writeResponse(MemPool* pool, const Request* request, Response* response) override;

    virtual int getRequestMethod(const Request* request) override;
};

} // namespace ngrest

#endif // NGREST_HTTPTRANSPORT_H
