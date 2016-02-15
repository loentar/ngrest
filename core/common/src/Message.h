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

namespace ngrest {

class Exception;
class Transport;
struct Node;
struct MessageContext;

struct Header
{
    const char* name;
    const char* value;
    const Header* next;

    inline Header(const char* name_ = nullptr, const char* value_ = nullptr, const Header* next_ = nullptr):
        name(name_), value(value_), next(next_)
    {
    }
};


struct Request
{
    const char* path = nullptr;
    const Header* headers = nullptr;

    char* body = nullptr;
    uint64_t bodySize = 0;

    Node* node = nullptr;

    // name must be in lower case
    const Header* getHeader(const char* name) const;
};

struct Response
{
    const Header* headers = nullptr;

    Node* node = nullptr;

    MemPool* poolBody = nullptr;
};

class MessageCallback: public VoidCallback
{
};

class Engine;
struct MessageContext
{
    Engine* engine = nullptr;
    Transport* transport = nullptr;
    Request* request = nullptr;
    Response* response = nullptr;
    MessageCallback* callback = nullptr;
    MemPool* pool = nullptr;
};

} // namespace ngrest

#endif // NGREST_MESSAGECONTEXT_H

