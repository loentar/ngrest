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

#include "ngrestcommonexport.h"

/**
 * Object Model (OM) is an internal representation of structured data.
 * ngrest's OM is based on JSON
 */

namespace ngrest {

/**
 * @brief node type
 */
enum class NodeType
{
    NamedNode,    //!< child of object with name
    LinkedNode,   //!< unnamed child node, for arrays
    Object,       //!< object node
    Array,        //!< array node
    Value         //!< value node
};

/**
 * @brief type of value node stored
 */
enum class ValueType
{
    Undefined,    //!< uninitialized, shouldn't exist in real OM
    NaN,          //!< not a number - invalid number
    String,       //!< string node type
    Number,       //!< number node type
    Boolean       //!< boolean node type
};

/**
 * @brief abstract node. parent for any inherited nodes. holds inherited node type
 * it doesn't have virtual destructor because it used as POCO,
 * also it as well as any inherited types are allocated and destroyed via memory pool
 */
struct NGREST_COMMON_EXPORT Node
{
    const NodeType type;    //<! node type @sa NodeType

    /**
     * @brief constructor
     * @param type_ inherited node type
     */
    inline Node(NodeType type_):
        type(type_)
    {
    }
};

/**
 * @brief linked node is to store array children. does not have a name
 */
struct NGREST_COMMON_EXPORT LinkedNode: public Node {
    Node* node;                         //!< array element
    LinkedNode* nextSibling = nullptr;  //!< pointer to the next sibling element in list (next array's child)

    /**
     * @brief constructor
     * @param node_ node of array element
     */
    inline LinkedNode(Node* node_ = nullptr):
        Node(NodeType::NamedNode),
        node(node_)
    {
    }
};

/**
 * @brief node with name to store object children, have a name
 */
struct NGREST_COMMON_EXPORT NamedNode: public Node {
    const char* name;                   //!< object child name (key)
    Node* node;                         //!< object child (value)
    NamedNode* nextSibling = nullptr;   //!< pointer to the next sibling element (next object's child)

    /**
     * @brief constructor
     * @param name_ object child name
     * @param node_ object child
     */
    inline NamedNode(const char* name_ = nullptr, Node* node_ = nullptr):
        Node(NodeType::NamedNode),
        name(name_),
        node(node_)
    {
    }
};

/**
 * @brief object node. represents JS object
 */
struct NGREST_COMMON_EXPORT Object: public Node
{
    NamedNode* firstChild = nullptr;    //!< pointer to the first child

    /**
     * @brief constructor
     */
    inline Object():
        Node(NodeType::Object)
    {
    }

    /**
     * @brief find child by name
     * @param name child's name (key)
     * @return pointer to found child or nullptr if no child with given name found
     */
    NamedNode* findChildByName(const char* name) const;
};

/**
 * @brief array node. represents JS array
 */
struct NGREST_COMMON_EXPORT Array: public Node
{
    LinkedNode* firstChild = nullptr;   //!< pointer to the first array element

    /**
     * @brief constructor
     */
    inline Array():
        Node(NodeType::Array)
    {
    }
};

/**
 * @brief value node. stores actual data
 */
struct NGREST_COMMON_EXPORT Value: public Node
{
    ValueType valueType;    //!< type of value @sa ValueType
    const char* value;      //!< stored value in C-string

    /**
     * @brief constructor
     * @param valueType_ type of value
     * @param value_ pointer to value
     */
    inline Value(ValueType valueType_, const char* value_ = nullptr):
        Node(NodeType::Value),
        valueType(valueType_),
        value(value_)
    {
    }
};

}

#endif // NGREST_OBJECTMODEL_H
