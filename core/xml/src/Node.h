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

#ifndef NGREST_XML_NODE_H
#define NGREST_XML_NODE_H

#include <string>
#include "Value.h"
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

class Element;

/**
 * @brief XML Node
     */
class NGREST_XML_EXPORT Node
{
public:
    enum class Type //! node type
    {
        Unknown,  //!< invalid/unknown type
        Element,  //!< element
        Comment,  //!< comment
        Text,     //!< text
        Cdata     //!< CDATA section
    };

public:
    /**
     * @brief destructor
     */
    virtual ~Node();

    /**
     * @brief get node type
     * @return node type
     */
    Type getType() const;

    /**
     * @brief get node value
     * @return node value
     */
    virtual const Value& getValue() const;

    /**
     * @brief get node value
     * @return node value
     */
    virtual const std::string& getTextValue() const;

    /**
     * @brief set node value
    * @param value new node value
     */
    virtual void setValue(const Value& value);

    /**
     * @brief get parent element
     * @return pointer to parent node or nullptr
     */
    const Element* getParent() const;

    /**
     * @brief get parent element
     * @return pointer to parent node or nullptr
     */
    Element* getParent();


    /**
     * @brief clone node
     * @return cloned node
     */
    Node* clone() const;


    /**
     * @brief get element of this node (only if node type = Element)
     * @return element
     */
    const Element& getElement() const;

    /**
     * @brief get element of this node (only if node type = Element)
     * @return element
     */
    Element& getElement();


    /**
     * @brief get next sibling node
     * @return next sibling node or nullptr if current node is last
     */
    const Node* getNextSibling() const;

    /**
     * @brief get next sibling node
     * @return next sibling node or nullptr if current node is last
     */
    Node* getNextSibling();

    /**
     * @brief get previous sibling node
     * @return previous sibling node or nullptr if current node is first
     */
    const Node* getPreviousSibling() const;

    /**
     * @brief get previous sibling node
     * @return previous sibling node or nullptr if current node is first
     */
    Node* getPreviousSibling();


    /**
     * @brief get next sibling element
     * @return next sibling element or nullptr if current element is last
     */
    const Element* getNextSiblingElement() const;

    /**
     * @brief get next sibling element
     * @return next sibling element or nullptr if current element is last
     */
    Element* getNextSiblingElement();

    /**
     * @brief get previous sibling element
     * @return previous sibling element or nullptr if current element is first
     */
    const Element* getPreviousSiblingElement() const;

    /**
     * @brief get previous sibling element
     * @return previous sibling element or nullptr if current element is first
     */
    Element* getPreviousSiblingElement();


    /**
     * @brief detach node from tree
     */
    Node* Detach();

protected:
    Node(Type type, Element* parent);
    Node(Type type, const Value& value, Element* parent);

private:
    Node(const Node& node);
    Node& operator=(const Node& node);

private:
    Type type;             //!< node type
    Value value;           //!< node value
    Element* parent;       //!< parent element
    Node* nextSibling;     //!< next sibling
    Node* previousSibling; //!< previous sibling
    friend class Element;
};


/**
 * @brief Comment node
 */
class NGREST_XML_EXPORT Comment: public Node
{
public:
    /**
     * @brief  constructor
     * @param parent parent element
     */
    Comment(Element* parent = nullptr);

    /**
     * @brief  constructor
     * @param value node value
     * @param parent parent element
     */
    Comment(const std::string& value, Element* parent = nullptr);
};


/**
 * @brief Text node
 */
class NGREST_XML_EXPORT Text: public Node
{
public:
    /**
     * @brief  constructor
     * @param parent parent element
     */
    Text(Element* parent = nullptr);

    /**
     * @brief  constructor
     * @param value node value
     * @param parent parent element
     */
    Text(const std::string& value, Element* parent = nullptr);
};


/**
 * @brief CDATA node
 */
class NGREST_XML_EXPORT Cdata: public Node
{
public:
    /**
     * @brief  constructor
     * @param parent parent element
     */
    Cdata(Element* parent = nullptr);

    /**
     * @brief  constructor
     * @param value node value
     * @param parent parent element
     */
    Cdata(const std::string& value, Element* parent = nullptr);
};

} // namespace xml
} // namespace ngrest

#endif // #ifndef NGREST_XML_NODE_H
