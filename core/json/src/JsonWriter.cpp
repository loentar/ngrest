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

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/MemPool.h>

#include <ngrest/common/ObjectModel.h>

#include "JsonWriter.h"

namespace ngrest {
namespace json {

class JsonWriterImpl {
public:
    MemPool& pool;
    int indent;

    inline JsonWriterImpl(MemPool& memPool, int indent_):
        pool(memPool),
        indent(indent_)
    {
    }

    inline void writeNode(const Node* node)
    {
        switch (node->type) {

        case NodeType::Object: {
            const Object* object = static_cast<const Object*>(node);
            pool.putChar('{');
            for (const NamedNode* child = object->firstChild; child; child = child->nextSibling) {
                if (child != object->firstChild)
                    pool.putChar(',');
                pool.putChar('"');
                pool.putCString(child->name);
                pool.putChar('"');
                pool.putChar(':');
                writeNode(child->node);
            }
            pool.putChar('}');
            break;
        }

        case NodeType::Array: {
            const Array* array = static_cast<const Array*>(node);
            pool.putChar('[');
            for (const LinkedNode* child = array->firstChild; child; child = child->nextSibling) {
                if (child != array->firstChild)
                    pool.putChar(',');
                writeNode(child->node);
            }
            pool.putChar(']');
            break;
        }

        case NodeType::Value: {
            const Value* value = static_cast<const Value*>(node);
            switch (value->valueType) {
            case ValueType::Undefined:
                pool.putData("undefined", 9);
                break;

            case ValueType::Null:
                pool.putData("null", 4);
                break;

            case ValueType::NaN:
                pool.putData("NaN", 3);
                break;

            case ValueType::String:
                pool.putChar('"');
                pool.putCString(value->value);
                pool.putChar('"');
                break;

            case ValueType::Number:
            case ValueType::Boolean:
                pool.putCString(value->value);
                break;
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

void JsonWriter::write(const Node* node, MemPool& memPool, int indent)
{
    NGREST_ASSERT(memPool.isClean(), "Mempool must be clean!");

    JsonWriterImpl(memPool, indent).writeNode(node);
}


}
}
