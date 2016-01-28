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

#ifndef NGREST_OBJECTMODEL_H
#define NGREST_OBJECTMODEL_H

namespace ngrest {

enum class NodeType
{
    NamedNode,
    LinkedNode,
    Object,
    Array,
    Value
};

enum class ValueType
{
    Undefined,
    Null,
    NaN,
    String,
    Number,
    Boolean
};

struct Node
{
    const NodeType type;

    inline Node(NodeType type_):
        type(type_)
    {
    }
};

struct LinkedNode: public Node {
    Node* node;
    LinkedNode* nextSibling = nullptr;

    inline LinkedNode(Node* node_ = nullptr):
        Node(NodeType::NamedNode),
        node(node_)
    {
    }
};

struct NamedNode: public Node {
    const char* name;
    Node* node;
    NamedNode* nextSibling = nullptr;

    inline NamedNode(const char* name_ = nullptr, Node* node_ = nullptr):
        Node(NodeType::NamedNode),
        name(name_),
        node(node_)
    {
    }
};

struct Object: public Node
{
    NamedNode* firstChild = nullptr;

    inline Object():
        Node(NodeType::Object)
    {
    }

    NamedNode* findChildByName(const char* name) const;
};

struct Array: public Node
{
    LinkedNode* firstChild = nullptr;

    inline Array():
        Node(NodeType::Array)
    {
    }
};

struct Value: public Node
{
    ValueType valueType;
    const char* value;

    inline Value(ValueType valueType_, const char* value_ = nullptr):
        Node(NodeType::Value),
        valueType(valueType_),
        value(value_)
    {
    }
};

}

#endif // NGREST_OBJECTMODEL_H
