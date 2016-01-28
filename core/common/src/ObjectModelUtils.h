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

#ifndef NGREST_OBJECTMODELUTILS_H
#define NGREST_OBJECTMODELUTILS_H

#include <string>
#include <ngrest/utils/fromcstring.h>
#include <ngrest/utils/Exception.h>
#include "ObjectModel.h"

namespace ngrest {

class ObjectModelUtils
{
public:
    static inline const NamedNode* getNamedChild(const Object* object, const char* name, NodeType type)
    {
        const NamedNode* namedNode = object->findChildByName(name);
        NGREST_ASSERT(namedNode, "Failed to get child " + std::string(name) + " is missing");
        NGREST_ASSERT(namedNode->node,
                      "Failed to read element " + std::string(name) + " node is null");
        NGREST_ASSERT(namedNode->node->type == type,
                      "Failed to read element " + std::string(name) + " node type does not match");
        return namedNode;
    }

    template <typename Type>
    static inline void getChildValue(const Object* object, const char* name, Type& value)
    {
        const NamedNode* namedNode = getNamedChild(object, name, NodeType::Value);
        const char* valueStr = static_cast<const Value*>(namedNode->node)->value;
        NGREST_ASSERT(valueStr, "Failed to read parameter " + std::string(name) + " is null");
        NGREST_ASSERT(::ngrest::fromCString(valueStr, value), "Failed to read parameter "
                      + std::string(name) + ": failed to convert from string");
    }

    static inline void getChildValue(const Object* object, const char* name, std::string& value)
    {
        value = getChildValue(object, name);
    }

    static inline const char* getChildValue(const Object* object, const char* name)
    {
        const NamedNode* namedNode = getNamedChild(object, name, NodeType::Value);
        const char* valueStr = static_cast<const Value*>(namedNode->node)->value;
        NGREST_ASSERT(valueStr, "Failed to read parameter " + std::string(name) + " is null");
        return valueStr;
    }

    template <typename Type>
    static inline void getValue(const Node* node, Type& value)
    {
        NGREST_ASSERT(node, "Failed to get value: value is null");
        NGREST_ASSERT(node->type == NodeType::Value,
                      "Failed to get value: node type does not match");
        const char* valueStr = static_cast<const Value*>(node)->value;
        NGREST_ASSERT_NULL(valueStr);
        NGREST_ASSERT(::ngrest::fromCString(valueStr, value),
                      "Failed to get value: failed to convert from string");
    }

    static inline void getValue(const Node* node, std::string& value)
    {
        value = getValue(node);
    }

    static inline const char* getValue(const Node* node)
    {
        NGREST_ASSERT(node, "Failed to get value: value is null");
        NGREST_ASSERT(node->type == NodeType::Value,
                      "Failed to get value: node type does not match");
        const char* valueStr = static_cast<const Value*>(node)->value;
        NGREST_ASSERT_NULL(valueStr);
        return valueStr;
    }
};

} // namespace ngrest

#endif // NGREST_OBJECTMODELUTILS_H
