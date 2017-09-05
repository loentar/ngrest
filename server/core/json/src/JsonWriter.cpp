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

#include "JsonWriter.h"

namespace ngrest {
namespace json {

class JsonWriterImpl {
public:
    MemPool* pool;
    int indent;

    inline JsonWriterImpl(MemPool* memPool, int indent_):
        pool(memPool),
        indent(indent_)
    {
    }

    inline char toHexChar(short dec)
    {
        static char hexTable[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
        return hexTable[dec];
    }

    inline void putString(const char* str)
    {
        pool->putChar('"');
#ifdef NGREST_JSON_NO_QUOTE_STRING
        pool->putCString(str);
#else
        size_t len = strlen(str);
        pool->reserve(len);
        const char* start = str;
        const char* curr = str;
        for (; *curr; ++curr) {
            if (*curr < 0x20) {
                if (start) {
                    if (curr > start) {
                        pool->putData(start, curr - start);
                    }
                }
                switch (*curr) {
                case '\b':
                    pool->putData("\\b", 2);
                    break;
                case '\f':
                    pool->putData("\\f", 2);
                    break;
                case '\n':
                    pool->putData("\\n", 2);
                    break;
                case '\r':
                    pool->putData("\\r", 2);
                    break;
                case '\t':
                    pool->putData("\\t", 2);
                    break;
                default:
                    pool->putData("\\u00", 4);
                    pool->putChar(toHexChar((*curr >> 4) & 0x0f));
                    pool->putChar(toHexChar(*curr & 0x0f));
                    break;
                }
                start = curr + 1;
            } else {
                if (*curr == '"' || *curr == '\\') {
                    if (start) {
                        if (curr > start) {
                            pool->putData(start, curr - start);
                        }
                        start = curr + 1;
                    }

                    pool->putChar('\\');
                    pool->putChar(*curr);
                }
            }
        }
        if (start && curr > start) {
            pool->putData(start, curr - start);
        }
#endif
        pool->putChar('"');
    }

    inline void writeNode(const Node* node)
    {
        if (!node) {
            pool->putData("null", 4);
            return;
        }

        switch (node->type) {

        case NodeType::Object: {
            const Object* object = static_cast<const Object*>(node);
            pool->putChar('{');
            for (const NamedNode* child = object->firstChild; child; child = child->nextSibling) {
                if (child != object->firstChild)
                    pool->putChar(',');
                putString(child->name);
                pool->putChar(':');
                writeNode(child->node);
            }
            pool->putChar('}');
            break;
        }

        case NodeType::Array: {
            const Array* array = static_cast<const Array*>(node);
            pool->putChar('[');
            for (const LinkedNode* child = array->firstChild; child; child = child->nextSibling) {
                if (child != array->firstChild)
                    pool->putChar(',');
                writeNode(child->node);
            }
            pool->putChar(']');
            break;
        }

        case NodeType::Value: {
            const Value* value = static_cast<const Value*>(node);
            switch (value->valueType) {
            case ValueType::NaN:
                pool->putData("NaN", 3);
                break;

            case ValueType::String:
                putString(value->value);
                break;

            case ValueType::Number:
            case ValueType::Boolean:
                pool->putCString(value->value);
                break;

            default:
                NGREST_THROW_ASSERT("Unexpected type of value node");
            }
            break;
        }

        case NodeType::NamedNode:
        case NodeType::LinkedNode: {
            NGREST_THROW_ASSERT("Cannot write Named or Linked node alone");
        }
        }
    }
};

void JsonWriter::write(const Node* node, MemPool* memPool, int indent)
{
    NGREST_ASSERT(memPool->isClean(), "Mempool must be clean!");

    JsonWriterImpl(memPool, indent).writeNode(node);
}


}
}
