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

#ifndef NGREST_TRANSPORT_H
#define NGREST_TRANSPORT_H

#include "ngrestengineexport.h"

namespace ngrest {

class MemPool;
struct Request;
struct Response;
struct Node;

/**
 * @brief abstract transport to translate request to OM and OM to response
 */
class NGREST_ENGINE_EXPORT Transport
{
public:
    /**
     * @brief transport type
     */
    enum class Type
    {
        Unknown,
        Http,
        User
    };

public:
    /**
     * @brief constructor
     * @param type transport type
     */
    Transport(Type type);

    /**
     * @brief destructor
     */
    virtual ~Transport();

    /**
     * @brief get transport type
     * @return transport type
     */
    inline Type getType() const
    {
        return type;
    }

    /**
     * @brief parse incoming transport request and translate it to OM request
     * @param pool pool to store temporary and OM data
     * @param request incoming request
     * @return OM translated from request
     */
    virtual Node* parseRequest(MemPool* pool, Request* request) = 0;

    /**
     * @brief translate OM response to transport response
     * @param pool pool to store temporary data
     * @param request source request
     * @param response response translated from OM
     */
    virtual void writeResponse(MemPool* pool, const Request* request, Response* response) = 0;

    /**
     * @brief get request method
     * @param request request
     * @return request method
     */
    virtual int getRequestMethod(const Request* request) = 0;

private:
    Type type;
};

} // namespace ngrest

#endif // NGREST_TRANSPORT_H
