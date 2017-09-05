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
#include "ngrestengineexport.h"

namespace ngrest {

/**
 * @brief provides serialization/deserialization for the message
 */
class NGREST_ENGINE_EXPORT HttpTransport: public Transport
{
public:
    /**
     * @brief constructor
     */
    HttpTransport();

    /**
     * @brief parse request body into OM
     * @param pool pool to store temporary data
     * @param request in/out request
     * @return parsed OM
     */
    virtual Node* parseRequest(MemPool* pool, Request* request) override;

    /**
     * @brief write response OM to the response body
     * @param pool pool to store temporary data
     * @param request in request
     * @param response out response
     */
    virtual void writeResponse(MemPool* pool, const Request* request, Response* response) override;

    /**
     * @brief get request method code from request
     * @param request request
     * @return method code
     */
    virtual int getRequestMethod(const Request* request) override;
};

} // namespace ngrest

#endif // NGREST_HTTPTRANSPORT_H
