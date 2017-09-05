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

#include <core/utils/Exception.h>
#include <core/utils/MemPool.h>

#include <ngrest/common/ObjectModel.h>

#include "JsonReader.h"

namespace ngrest {
namespace json {

class JsonReaderImpl {
public:
    int line = 1;
    char* begin;
    char* curr;
    MemPool* pool;

    inline void skipWs()
    {
        for (;; ++curr) {
            if (*curr == '\n') {
                ++line;
            } else if (*curr != ' ' && *curr != '\r' && *curr != '\t') {
                break;
            }
        }
    }

    inline bool seekTo(char ch)
    {
        for (; *curr != '\0'; ++curr)
            if (*curr == ch)
                return true;
        return false;
    }

    inline char* tokenValue() {
        char* start = curr;
        while (*curr != '}' &&
               *curr != ']' &&
               *curr != ',' &&
               *curr != ' ' &&
               *curr != '\r' &&
               *curr != '\t') {
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading token");

            if (*curr == '\n') {
                ++line;
                break;
            }
            ++curr;
        }
        // avoid parsing error. terminate token within readObject or readArray
        // *curr = '\0';
        // ++curr;
        return start;
    }

#ifdef NGREST_JSON_NO_QUOTE_STRING
    inline char* tokenString() {
        ++curr; // skip '"'
        char* start = curr;
        while (*curr != '"') {
            if (*curr == '\\') { // skip '\"'
                ++curr;
                if (*curr == '"')
                    ++curr;
            }
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading token");

            if (*curr == '\n') {
                ++line;
                break;
            }
            ++curr;
        }
        *curr = '\0';
        ++curr;
        return start;
    }
#else

    inline char fromHexChar(short hex)
    {
        static const char table[256] = {
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x00
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x10
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x20
            0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x30
            0x10, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x40
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x50
            0x10, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x60
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x70
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x80
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // x90
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // xA0
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // xB0
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // xC0
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // xD0
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, // xE0
            0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x10  // xF0
        };
        NGREST_ASSERT(table[hex] != 0x10, "Unexpected character while reading unicode hex digit");
        return table[hex];
    }

    inline char* tokenString() {
        ++curr; // skip '"'
        char* start = curr;
        char* out = nullptr;
        while (*curr != '"') {
            if (*curr == '\\') { // parse quoted characters
                if (!out) {
                    out = curr;
                }
                ++curr;
                switch (*curr) {
                case 'b':
                    *out = '\b';
                    break;
                case 'f':
                    *out = '\f';
                    break;
                case 'n':
                    *out = '\n';
                    break;
                case 'r':
                    *out = '\r';
                    break;
                case 't':
                    *out = '\t';
                    break;
                case 'u':
                    ++curr;
                    *out = fromHexChar(*curr) << 4;
                    ++curr;
                    *out |= fromHexChar(*curr);
                    if (*out) { // \u00XX
                        ++out;
                    }
                    ++curr;
                    *out = fromHexChar(*curr) << 4;
                    ++curr;
                    *out |= fromHexChar(*curr);
                    break;
                default:
                    *out = *curr;
                    break;
                }
                ++out;
            } else {
                NGREST_ASSERT(*curr >= 0x20, "Unexpected control character while reading string");
                if (out) {
                    *out = *curr;
                    ++out;
                }
            }
            ++curr;
        }
        if (out) {
            *out = '\0';
        } else {
            *curr = '\0';
        }
        ++curr;
        return start;
    }
#endif


    inline JsonReaderImpl(char* buff, MemPool* memPool):
        begin(buff),
        curr(buff),
        pool(memPool)
    {
    }

    inline Node* readAny()
    {
        skipWs();

        switch (*curr) {
        case '[':
            return readArray();

        case '{':
            return readObject();

        case '"': // string
            return pool->alloc<Value>(ValueType::String, tokenString());
        }

        // number or special value
        const char* token = tokenValue();
        const int len = curr - token;

        // number
        if ((*token >= '0' && *token <= '9') || *token == '-')
            return pool->alloc<Value>(ValueType::Number, token);

        if (!strncmp(token, "true", len) || !strncmp(token, "false", len))
            return pool->alloc<Value>(ValueType::Boolean, token);

        // handle undefined, NaN, null
        if (!strncmp(token, "null", len))
            return nullptr;

        if (!strncmp(token, "NaN", len))
            return pool->alloc<Value>(ValueType::NaN);

        NGREST_THROW_ASSERT(std::string("Unexpected token: [") + token + "]");
    }

    inline Node* readArrayOrObject()
    {
        skipWs();

        switch (*curr) {
        case '[':
            return readArray();
        case '{':
            return readObject();
        }

        NGREST_THROW_ASSERT(std::string("Unexpected symbol: [") + *curr + "]");
    }

    inline Array* readArray()
    {
        ++curr; // skip '['

        Array* array = pool->alloc<Array>();

        skipWs();
        // empty array
        if (*curr == ']') {
            ++curr;
            return array;
        }

        LinkedNode* prevLinkedNode = nullptr;
        LinkedNode* linkedNode;
        char* valueEnd;

        for (;;) {
            linkedNode = pool->alloc<LinkedNode>(readAny());

            if (prevLinkedNode == nullptr) {
                array->firstChild = linkedNode;
            } else {
                prevLinkedNode->nextSibling = linkedNode;
            }
            prevLinkedNode = linkedNode;

            valueEnd = (linkedNode->node && (linkedNode->node->type == NodeType::Value)) ? curr : nullptr;
            skipWs();
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading array");

            if (*curr == ']') {
                // terminate token
                if (valueEnd)
                    *valueEnd = '\0';
                ++curr;
                return array;
            }
            NGREST_ASSERT(*curr == ',', "Missing ',' while reading array");
            if (valueEnd)
                *valueEnd = '\0';
            ++curr;
        }
    }

    inline Object* readObject()
    {
        // {"name": ...., "name2":...}
        ++curr; // skip '{'

        Object* object = pool->alloc<Object>();

        skipWs();
        // empty object
        if (*curr == '}') {
            ++curr;
            return object;
        }

        NamedNode* prevNamedNode = nullptr;
        NamedNode* namedNode;
        char* valueEnd;

        for (;;) {
            NGREST_ASSERT(*curr == '"', "Missing '\"' while reading object name");
            namedNode = pool->alloc<NamedNode>(tokenString());

            skipWs();
            NGREST_ASSERT(*curr == ':', "Missing ':' after object name");
            ++curr;

            namedNode->node = readAny();

            if (prevNamedNode == nullptr) {
                object->firstChild = namedNode;
            } else {
                prevNamedNode->nextSibling = namedNode;
            }
            prevNamedNode = namedNode;

            valueEnd = (namedNode->node && (namedNode->node->type == NodeType::Value)) ? curr : nullptr;
            skipWs();
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading object");

            if (*curr == '}') {
                if (valueEnd)
                    *valueEnd = '\0';
                ++curr;
                return object;
            }
            NGREST_ASSERT(*curr == ',', "Missing ',' while reading object");
            if (valueEnd)
                *valueEnd = '\0';
            ++curr;
            skipWs();
        }
    }
};

Node* JsonReader::read(char* buff, MemPool* memPool)
{
    return JsonReaderImpl(buff, memPool).readArrayOrObject();
}


}
}
