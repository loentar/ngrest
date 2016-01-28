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

#include "Exception.h"
#include "Element.h"
#include "Node.h"

namespace ngrest {
namespace xml {

Node::Node(Type type_, Element* parent_):
    type(type_),
    parent(parent_),
    nextSibling(nullptr),
    previousSibling(nullptr)
{
    if (parent_)
        parent_->appendChild(this);
}

Node::Node(Type type_, const Value& value_, Element* parent_):
    type(type_),
    value(value_),
    parent(parent_),
    nextSibling(nullptr),
    previousSibling(nullptr)
{
    if (parent_)
        parent_->appendChild(this);
}

Node::~Node()
{
    // detach only if parent set
    if (parent)
        Detach();
}


Node::Type Node::getType() const
{
    return type;
}

const Value& Node::getValue() const
{
    return value;
}

const std::string& Node::getTextValue() const
{
    return value.asString();
}

void Node::setValue(const Value& value)
{
    this->value = value;
}

const Element* Node::getParent() const
{
    return parent;
}

Element* Node::getParent()
{
    return parent;
}

Node* Node::clone() const
{
    Node* clone = nullptr;
    switch (type) {
    case Type::Comment:
        clone = new Comment(value);
        break;
    case Type::Text:
        clone = new Text(value);
        break;
    case Type::Cdata:
        clone = new Cdata(value);
        break;
    case Type::Element:
        clone = reinterpret_cast<const Element*>(this)->cloneElement();
        break;
    default:
        NGREST_THROW_ASSERT("Can't create node clone");
    }

    return clone;
}

const Element& Node::getElement() const
{
    NGREST_ASSERT(type == Type::Element, "This node is not an Element");
    return static_cast<const Element&>(*this);
}

Element& Node::getElement()
{
    NGREST_ASSERT(type == Type::Element, "This node is not an Element");
    return static_cast<Element&>(*this);
}


const Node* Node::getNextSibling() const
{
    return nextSibling;
}

Node* Node::getNextSibling()
{
    return nextSibling;
}

const Node* Node::getPreviousSibling() const
{
    return previousSibling;
}

Node* Node::getPreviousSibling()
{
    return previousSibling;
}

const Element* Node::getNextSiblingElement() const
{
    const Node* node = nextSibling;
    for (; node && node->getType() != Type::Element; node = node->nextSibling);
    return reinterpret_cast<const Element*>(node);
}

Element* Node::getNextSiblingElement()
{
    Node* node = nextSibling;
    for (; node && node->getType() != Type::Element; node = node->nextSibling);
    return reinterpret_cast<Element*>(node);
}

const Element* Node::getPreviousSiblingElement() const
{
    const Node* node = previousSibling;
    for (; node && node->getType() != Type::Element; node = node->previousSibling);
    return reinterpret_cast<const Element*>(node);
}

Element* Node::getPreviousSiblingElement()
{
    Node* node = previousSibling;
    for (; node && node->getType() != Type::Element; node = node->previousSibling);
    return reinterpret_cast<Element*>(node);
}

Node* Node::Detach()
{
    if (parent) {
        if (parent->firstChild == this)
            parent->firstChild = nextSibling;
        if (parent->lastChild == this)
            parent->lastChild = previousSibling;
        parent = nullptr;
    }
    if (nextSibling)
        nextSibling->previousSibling = previousSibling;
    if (previousSibling)
        previousSibling->nextSibling = nextSibling;
    nextSibling = nullptr;
    previousSibling = nullptr;

    return this;
}


Comment::Comment(Element* parent):
    Node(Type::Comment, parent)
{
}

Comment::Comment(const std::string& value, Element* parent):
    Node(Type::Comment, value, parent)
{
}


Text::Text(Element* parent):
    Node(Type::Text, parent)
{
}

Text::Text(const std::string& value, Element* parent):
    Node(Type::Text, value, parent)
{
}


Cdata::Cdata(Element* parent):
    Node(Type::Cdata, parent)
{
}

Cdata::Cdata(const std::string& value, Element* parent):
    Node(Type::Cdata, value, parent)
{
}

} // namespace xml
} // namespace ngrest
