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

#ifndef NGREST_MESSAGECONTEXT_H
#define NGREST_MESSAGECONTEXT_H

#include <stdint.h>

#include <ngrest/common/Callback.h>
#include <ngrest/utils/MemPool.h>
#include "ngrestcommonexport.h"

namespace ngrest {

class Exception;
class Transport;
struct Node;
struct MessageContext;

/**
 * @brief request/response header
 */
struct NGREST_COMMON_EXPORT Header
{
    const char* name;       //!< header name
    const char* value;      //!< header value
    Header* next;           //!< pointer to next header

    inline Header(const char* name_ = nullptr, const char* value_ = nullptr, Header* next_ = nullptr):
        name(name_), value(value_), next(next_)
    {
    }
};


/**
 * @brief request
 */
struct NGREST_COMMON_EXPORT Request
{
    const char* path = nullptr;         //!< path to the REST resource
    Header* headers = nullptr;          //!< list of headers passed from client

    char* body = nullptr;               //!< request body
    uint64_t bodySize = 0;              //!< size of request body
    MemPool* poolBody = nullptr;        //!< pool where request body is stored or nullptr when no separate pool used

    Node* node = nullptr;               //!< parsed body

    /**
     * @brief getHeader gets header by name
     * @param name header name in lower case
     * @return pointer to header, or nullptr of no header matched
     */
    const Header* getHeader(const char* name) const;

    /**
     * @brief getHeader gets header by name
     * @param name header name in lower case
     * @return pointer to header, or nullptr of no header matched
     */
    Header* getHeader(const char* name);
};

/**
 * @brief response
 */
struct NGREST_COMMON_EXPORT Response
{
    Header* headers = nullptr;          //!< list of headers to send to client

    Node* node = nullptr;               //!< response body node

    MemPool* poolBody = nullptr;        //!< response body
};

/**
 * @brief message callback
 */
class NGREST_COMMON_EXPORT MessageCallback: public VoidCallback
{
};

class Engine;

/**
 * @brief message context
 */
struct NGREST_COMMON_EXPORT MessageContext
{
    Engine* engine = nullptr;               //!< engine used to process this message
    Transport* transport = nullptr;         //!< transport used to receive and send this message
    Request* request = nullptr;             //!< request
    Response* response = nullptr;           //!< response
    MessageCallback* callback = nullptr;    //!< callback to send message after processing
    MemPool* pool = nullptr;                //!< pool to store temporary data upon message processing
};

} // namespace ngrest

#endif // NGREST_MESSAGECONTEXT_H

