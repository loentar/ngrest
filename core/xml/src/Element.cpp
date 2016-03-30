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

#include <iostream>
#include "Element.h"
#include "Attribute.h"
#include "Namespace.h"
#include "Exception.h"

namespace ngrest {
namespace xml {

Element::Element(Element* parent):
    Node(Type::Element, parent),
    firstAttribute(nullptr),
    firstNamespace(nullptr),
    firstChild(nullptr),
    lastChild(nullptr)
{
}

Element::Element(const std::string& name, Element* parent):
    Node(Type::Element, parent),
    firstAttribute(nullptr),
    firstNamespace(nullptr),
    firstChild(nullptr),
    lastChild(nullptr)
{
    setName(name);
}

Element::Element(const std::string& name, const Value& value, Element* parent):
    Node(Type::Element, parent),
    firstAttribute(nullptr),
    firstNamespace(nullptr),
    firstChild(nullptr),
    lastChild(nullptr)
{
    setName(name);
    createText(value);
}

Element::Element(const Element& element):
    Node(Type::Element, nullptr),
    firstAttribute(nullptr),
    firstNamespace(nullptr),
    firstChild(nullptr),
    lastChild(nullptr)
{
    cloneElement(element);
}

Element& Element::operator=(const Element& element)
{
    clear();
    return cloneElement(element);
}

Element::~Element()
{
    clear();
}

Element* Element::cloneElement(bool recursive /*= true*/) const
{
    Element* clone = nullptr;
    switch (type) {
    case Type::Element:
        clone = new Element;
        break;
    default:
        NGREST_THROW_ASSERT("Can't create node clone");
    }

    try {
        clone->cloneElement(*this, recursive);
    } catch (...) {
        delete clone;
        throw;
    }

    return clone;
}

Element& Element::cloneElement(const Element& element, bool recursive)
{
    name = element.name;
    prefix = element.prefix;

    // clone attributes
    if (element.firstAttribute) {
        Attribute* newAttribute = new Attribute(*element.firstAttribute);
        firstAttribute = newAttribute;

        for (const Attribute* attribute = element.firstAttribute->nextSibling;
             attribute; attribute = attribute->nextSibling) {
            newAttribute->nextSibling = new Attribute(*attribute);
            newAttribute = newAttribute->nextSibling;
        }
    }

    // clone namespaces
    if (element.firstNamespace) {
        // default namespace
        Namespace* newNs = new Namespace(*element.firstNamespace);
        firstNamespace = newNs;

        for (const Namespace* ns = element.firstNamespace->nextSibling; ns; ns = ns->nextSibling) {
            newNs->nextSibling = new Namespace(*ns);
            newNs = newNs->nextSibling;
        }

        prefix = element.prefix;
    }

    // clone children
    if (recursive) {
        for (const Node* child = element.firstChild; child; child = child->nextSibling) {
            appendChild(child->clone());
        }
    }

    return *this;
}

const std::string& Element::getName() const
{
    return name;
}

void Element::setName(const std::string& name)
{
    std::string::size_type pos = name.find_last_of(':');
    if (pos == std::string::npos) {
        this->name = name;
        prefix.erase();
    } else {
        this->name.assign(name, pos + 1, std::string::npos);
        prefix.assign(name, 0, pos);
    }
}

const std::string& Element::getPrefix() const
{
    return prefix;
}

std::string Element::getPrefixName() const
{
    return prefix.empty() ? name : (prefix + ":" + name);
}

std::string Element::GetChildrenText() const
{
    std::string text;
    for (const Node* child = firstChild; child; child = child->nextSibling)
        if (child->getType() == Type::Text)
            text += child->getValue().asString();
    return text;
}

const Value& Element::getValue() const
{
    static const Value empty;

    for (const Node* child = firstChild; child; child = child->nextSibling)
        if (child->getType() == Type::Text)
            return child->getValue();

    return empty;
}

const std::string& Element::getTextValue() const
{
    static const std::string empty;

    for (const Node* child = firstChild; child; child = child->nextSibling)
        if (child->getType() == Type::Text)
            return child->getTextValue();

    return empty;
}

void Element::setValue(const Value& value)
{
    Node* childText = nullptr;
    {
        Node* childToRemove = nullptr;
        for (Node* child = firstChild; child; child = child->nextSibling) {
            if (childToRemove) {
                delete childToRemove;
                childToRemove = nullptr;
            }

            if (child->getType() == Type::Text) {
                if (childText) {
                    childToRemove = child;
                } else {
                    childText = child;
                }
            }
        }
        if (childToRemove) {
            delete childToRemove;
        }
    }

    if (childText) {
        childText->setValue(value);
    } else {
        createText(value);
    }
}

bool Element::isTextNull() const
{
    const Node* child = firstChild;
    for (; child && child->getType() != Type::Text; child = child->nextSibling);
    return !child;
}

void Element::setTextIsNull()
{
    Node* childToRemove = nullptr;
    for (Node* child = firstChild; child;) {
        if (child->getType() == Type::Text) {
            childToRemove = child;
            child = child->nextSibling;
            delete childToRemove;
        } else {
            child = child->nextSibling;
        }
    }
}

bool Element::isEmpty() const
{
    return !firstChild ||
            ((firstChild == lastChild) && (firstChild->type == Type::Text)
             && firstChild->getValue().asString().empty());
}

bool Element::isLeaf() const
{
    return !firstChild ||
            ((firstChild == lastChild) && (firstChild->type == Type::Text));
}

void Element::clear()
{
    if (firstAttribute) {
        // destroy attributes
        Attribute* attribute = firstAttribute;
        Attribute* prevAttribute = nullptr;
        while (attribute) {
            prevAttribute = attribute;
            attribute = attribute->nextSibling;
            delete prevAttribute;
        }
    }

    if (firstNamespace) {
        // destroy namespaces
        Namespace* ns = firstNamespace;
        Namespace* prevNs = nullptr;
        while (ns) {
            prevNs = ns;
            ns = ns->nextSibling;
            delete prevNs;
        }
    }

    if (firstChild) {
        Node* childToRemove = nullptr;
        for (Node* child = firstChild; child;) {
            childToRemove = child;
            child = child->nextSibling;

            childToRemove->parent = nullptr; // prevent unneeded detaching
            delete childToRemove;
        }
        firstChild = nullptr;
        lastChild = nullptr;
    }
}

// children management

Element& Element::createElement()
{
    return appendChild(new Element());
}

Element& Element::createElement(const std::string& name)
{
    return appendChild(new Element(name));
}

Element& Element::createElement(const std::string& name, const Value& value)
{
    return appendChild(new Element(name, value));
}

Element& Element::createElementOnce(const std::string& name)
{
    Element* child = findChildElementByName(name);
    if (child)
        return *child;

    return appendChild(new Element(name));
}

Comment& Element::createComment()
{
    return appendChild(new Comment());
}

Comment& Element::createComment(const Value& value)
{
    return appendChild(new Comment(value));
}

Text& Element::createText()
{
    return appendChild(new Text());
}

Text& Element::createText(const Value& value)
{
    return appendChild(new Text(value));
}

Cdata& Element::createCdata()
{
    return appendChild(new Cdata());
}

Cdata& Element::createCdata(const Value& value)
{
    return appendChild(new Cdata(value));
}

Node& Element::insertChildBefore(Node* node, Node* before)
{
    NGREST_ASSERT_PARAM(node);
    NGREST_ASSERT(node != this, "Can't append self as child");
    NGREST_ASSERT(!node->parent && !node->nextSibling && !node->previousSibling,
                  "Can't append node that is already in tree");

    NGREST_ASSERT_PARAM(before);
    NGREST_ASSERT(before->getParent() == this, "The 'Before' node belong to the different parent");
    NGREST_DEBUG_ASSERT(firstChild && lastChild, "Internal error");

    if (before == firstChild) {
        firstChild = node;
    } else {
        node->previousSibling = before->previousSibling;
        before->previousSibling->nextSibling = node;
    }
    node->nextSibling = before;
    before->previousSibling = node;

    node->parent = this;

    return *node;
}

Element& Element::insertChildBefore(Element* node, Node* before)
{
    return static_cast<Element&>(insertChildBefore(reinterpret_cast<Node*>(node), before));
}

Comment& Element::insertChildBefore(Comment* node, Node* before)
{
    return static_cast<Comment&>(insertChildBefore(reinterpret_cast<Node*>(node), before));
}

Text& Element::insertChildBefore(Text* node, Node* before)
{
    return static_cast<Text&>(insertChildBefore(reinterpret_cast<Node*>(node), before));
}

Cdata& Element::insertChildBefore(Cdata* node, Node* before)
{
    return static_cast<Cdata&>(insertChildBefore(reinterpret_cast<Node*>(node), before));
}

Node& Element::insertChildAfter(Node* node, Node* after)
{
    NGREST_ASSERT_PARAM(node);
    NGREST_ASSERT(node != this, "Can't append self as child");
    NGREST_ASSERT(!node->parent && !node->nextSibling && !node->previousSibling,
                  "Can't append node that is already in tree");

    NGREST_ASSERT_PARAM(after);
    NGREST_ASSERT(after->getParent() == this, "The 'After' node belong to the different parent");
    NGREST_DEBUG_ASSERT(firstChild && lastChild, "Internal error");

    if (after == lastChild) {
        lastChild = node;
    } else {
        node->nextSibling = after->nextSibling;
        after->nextSibling->previousSibling = node;
    }
    node->previousSibling = after;
    after->nextSibling = node;

    node->parent = this;

    return *node;
}

Element& Element::insertChildAfter(Element* node, Node* after)
{
    return static_cast<Element&>(insertChildAfter(reinterpret_cast<Node*>(node), after));
}

Comment& Element::insertChildAfter(Comment* node, Node* after)
{
    return static_cast<Comment&>(insertChildAfter(reinterpret_cast<Node*>(node), after));
}

Text& Element::insertChildAfter(Text* node, Node* after)
{
    return static_cast<Text&>(insertChildAfter(reinterpret_cast<Node*>(node), after));
}

Cdata& Element::insertChildAfter(Cdata* node, Node* after)
{
    return static_cast<Cdata&>(insertChildAfter(reinterpret_cast<Node*>(node), after));
}

Node& Element::appendChild(Node* node)
{
    NGREST_ASSERT_PARAM(node);
    NGREST_ASSERT(node != this, "Can't append self as child");
    NGREST_ASSERT(!node->parent && !node->nextSibling && !node->previousSibling,
                  "Can't append node that is already in tree");

    if (!firstChild || !lastChild) {
        NGREST_DEBUG_ASSERT(!firstChild && !lastChild, "Internal error");
        firstChild = node;
    } else {
        // append
        NGREST_DEBUG_ASSERT(!lastChild->nextSibling, "Internal error");

        lastChild->nextSibling = node;
        node->previousSibling = lastChild;
    }

    node->parent = this;
    lastChild = node;

    return *node;
}

Element& Element::appendChild(Element* node)
{
    return static_cast<Element&>(appendChild(reinterpret_cast<Node*>(node)));
}

Comment& Element::appendChild(Comment* node)
{
    return static_cast<Comment&>(appendChild(reinterpret_cast<Node*>(node)));
}

Text& Element::appendChild(Text* node)
{
    return static_cast<Text&>(appendChild(reinterpret_cast<Node*>(node)));
}

Cdata& Element::appendChild(Cdata* node)
{
    return static_cast<Cdata&>(appendChild(reinterpret_cast<Node*>(node)));
}

void Element::removeChild(Node* node)
{
    NGREST_ASSERT_PARAM(node);
    NGREST_ASSERT(node->getParent() == this, "This child node belong to the different parent");
    delete node;
}

const Node* Element::getFirstChild() const
{
    return firstChild;
}

Node* Element::getFirstChild()
{
    return firstChild;
}

const Node* Element::getLastChild() const
{
    return lastChild;
}

Node* Element::getLastChild()
{
    return lastChild;
}

const Element* Element::getFirstChildElement() const
{
    const Node* node = firstChild;
    for (; node && node->getType() != Type::Element; node = node->nextSibling);
    return reinterpret_cast<const Element*>(node);
}

Element* Element::getFirstChildElement()
{
    Node* node = firstChild;
    for (; node && node->getType() != Type::Element; node = node->nextSibling);
    return reinterpret_cast<Element*>(node);
}

const Element* Element::getLastChildElement() const
{
    const Node* node = lastChild;
    for (; node && node->getType() != Type::Element; node = node->getPreviousSibling());
    return reinterpret_cast<const Element*>(node);
}

Element* Element::getLastChildElement()
{
    Node* node = lastChild;
    for (; node && node->getType() != Type::Element; node = node->getPreviousSibling());
    return reinterpret_cast<Element*>(node);
}

const Element* Element::findChildElementByName(const std::string& name) const
{
    const Element* firstChild = getFirstChildElement();
    return firstChild ? findChildElementByName(name, firstChild) : nullptr;
}

Element* Element::findChildElementByName(const std::string& name)
{
    Element* firstChild = getFirstChildElement();
    return firstChild ? findChildElementByName(name, firstChild) : nullptr;
}

const Element* Element::findChildElementByName(const std::string& name, const Element* begin) const
{
    if (begin) {
        NGREST_ASSERT(begin->getParent() == this, "This child node belong to the different parent");

        for (; begin && begin->name != name; begin = begin->getNextSiblingElement());
    }
    return begin;
}

Element* Element::findChildElementByName(const std::string& name, Element* begin)
{
    if (begin) {
        NGREST_ASSERT(begin->getParent() == this, "This child node belong to the different parent");

        for (; begin && begin->name != name; begin = begin->getNextSiblingElement());
    }
    return begin;
}

const Element& Element::getChildElementByName(const std::string& name) const
{
    const Element* element = findChildElementByName(name);
    NGREST_ASSERT(element, "Child element with name [" + name + "] does not exists");
    return *element;
}

Element& Element::getChildElementByName(const std::string& name)
{
    Element* element = findChildElementByName(name);
    NGREST_ASSERT(element, "Child element with name [" + name + "] does not exists");
    return *element;
}

unsigned Element::getChildrenCount() const
{
    unsigned result = 0;
    for (Node* node = firstChild; node; node = node->nextSibling, ++result);
    return result;
}

unsigned Element::getChildrenElementsCount() const
{
    unsigned result = 0;
    for (Node* node = firstChild; node; node = node->nextSibling)
        if (node->getType() == Type::Element)
            ++result;
    return result;
}


// attributes management

Attribute& Element::createAttribute(const std::string& name, const std::string& value)
{
    return appendAttribute(new Attribute(name, value));
}

Attribute& Element::createAttribute(const std::string& name, const std::string& value,
                                    const std::string& prefix)
{
    return appendAttribute(new Attribute(name, value, prefix));
}

Attribute& Element::appendAttribute(const Attribute& attribute)
{
    return appendAttribute(new Attribute(attribute));
}

Attribute& Element::appendAttribute(Attribute* attribute)
{
    NGREST_ASSERT_PARAM(attribute);

    if (!firstAttribute) {
        // there is no attributes yet.
        firstAttribute = attribute;
    } else {
        // searching for last attribute
        Attribute* lastAttribute = firstAttribute;
        for (; lastAttribute->nextSibling; lastAttribute = lastAttribute->nextSibling);
        lastAttribute->nextSibling = attribute;
    }

    return *attribute;
}

const Attribute* Element::findAttribute(const std::string& name) const
{
    const Attribute* attribute = firstAttribute;
    for (; attribute && attribute->getName() != name; attribute = attribute->nextSibling);
    return attribute;
}

Attribute* Element::findAttribute(const std::string& name)
{
    Attribute* attribute = firstAttribute;
    for (; attribute && attribute->getName() != name; attribute = attribute->nextSibling);
    return attribute;
}

const Attribute& Element::getAttribute(const std::string& name) const
{
    const Attribute* attribute = findAttribute(name);
    NGREST_ASSERT(attribute, "Attribute with name [" + name + "] does not exists");
    return *attribute;
}

Attribute& Element::getAttribute(const std::string& name)
{
    Attribute* attribute = findAttribute(name);
    NGREST_ASSERT(attribute, "Attribute with name [" + name + "] does not exists");
    return *attribute;
}

const std::string& Element::getAttributeValue(const std::string& name) const
{
    return getAttribute(name).getValue();
}

void Element::setAttributeValue(const std::string& name, const std::string& value)
{
    getAttribute(name).setValue(value);
}

void Element::removeAttribute(Attribute* attribute)
{
    NGREST_ASSERT(firstAttribute, "There is no attributes in element");

    if (firstAttribute == attribute) {
        firstAttribute = firstAttribute->nextSibling;
    } else {
        // searching for attribute to remove
        Attribute* prevAttribute = firstAttribute;
        for (; prevAttribute->nextSibling && prevAttribute->nextSibling != attribute;
             prevAttribute = prevAttribute->nextSibling);

        NGREST_ASSERT(prevAttribute->nextSibling,
                      "Attribute with name [" + attribute->getName() + "] does not exists");

        prevAttribute->nextSibling = attribute->nextSibling;
    }
    delete attribute;
}

void Element::removeAttributeByName(const std::string& name)
{
    NGREST_ASSERT(firstAttribute, "There is no attributes in element");

    Attribute* attributeToRemove = nullptr;
    if (firstAttribute->getName() == name) {
        attributeToRemove = firstAttribute;
        firstAttribute = firstAttribute->nextSibling;
    } else {
        // searching for attribute to remove
        Attribute* prevAttribute = firstAttribute;
        attributeToRemove = prevAttribute->nextSibling;
        for (; attributeToRemove && attributeToRemove->getName() != name;
             prevAttribute = attributeToRemove, attributeToRemove = attributeToRemove->nextSibling);

        prevAttribute->nextSibling = attributeToRemove->nextSibling;
    }

    NGREST_ASSERT(attributeToRemove, "Attribute with name [" + name + "] does not exists");
    delete attributeToRemove;
}

bool Element::isAttributeExists(const std::string& name) const
{
    return !!findAttribute(name);
}


const Attribute* Element::getFirstAttribute() const
{
    return firstAttribute;
}

Attribute* Element::getFirstAttribute()
{
    return firstAttribute;
}



// namespaces management

Namespace& Element::declareDefaultNamespace(const std::string& uri)
{
    return declareNamespace(uri, "");
}

Namespace& Element::declareNamespace(const std::string& uri, const std::string& prefix)
{
    Namespace* prevNs = nullptr;
    Namespace* ns = firstNamespace;
    for (; ns && prefix != ns->getPrefix(); prevNs = ns, ns = ns->nextSibling);
    if (ns) {
        ns->setUri(uri);
    } else {
        ns = new Namespace(prefix, uri);
        if (prevNs) {
            prevNs->nextSibling = ns;
        } else {
            firstNamespace = ns;
        }
    }

    return *ns;
}

Namespace& Element::declareNamespace(const Namespace& ns)
{
    return declareNamespace(ns.uri, ns.prefix);
}

Namespace& Element::setNamespace(const std::string& uri, const std::string& prefix,
                                 bool recursive /*= true*/)
{
    replacePrefix(prefix, recursive);
    return declareNamespace(uri, prefix);
}

Namespace& Element::setNamespace(const Namespace& ns,
                                 bool recursive /*= true*/)
{
    replacePrefix(ns.prefix, recursive);
    return declareNamespace(ns.uri, ns.prefix);
}

const Namespace* Element::getNamespace() const
{
    return findNamespaceByPrefix(prefix);
}

Namespace* Element::getNamespace()
{
    return findNamespaceByPrefix(prefix);
}

const std::string& Element::getNamespacePrefixByUri(const std::string& uri)
{
    const Namespace* ns = findNamespaceByUri(uri);
    NGREST_ASSERT(ns, "Namespace with URI [" + uri + "] is not declared in current element");
    return ns->getPrefix();
}

const std::string& Element::getNamespaceUriByPrefix(const std::string& prefix)
{
    const Namespace* ns = findNamespaceByPrefix(prefix);
    NGREST_ASSERT(ns, "Namespace with prefix [" + prefix + "] is not declared in current element");
    return ns->getUri();
}

const Namespace* Element::findNamespaceByUri(const std::string& uri) const
{
    const Namespace* ns = firstNamespace;
    for (; ns && uri != ns->getUri(); ns = ns->nextSibling);
    return ns;
}

Namespace* Element::findNamespaceByUri(const std::string& uri)
{
    Namespace* ns = firstNamespace;
    for (; ns && uri != ns->getUri(); ns = ns->nextSibling);
    return ns;
}

const Namespace* Element::findNamespaceByPrefix(const std::string& prefix) const
{
    const Namespace* ns = firstNamespace;
    for (; ns && prefix != ns->getPrefix(); ns = ns->nextSibling);
    return ns;
}

Namespace* Element::findNamespaceByPrefix(const std::string& prefix)
{
    Namespace* ns = firstNamespace;
    for (; ns && prefix != ns->getPrefix(); ns = ns->nextSibling);
    return ns;
}

const Namespace* Element::getFirstNamespace() const
{
    return firstNamespace;
}

Namespace* Element::getFirstNamespace()
{
    return firstNamespace;
}

void Element::findElementNamespaceDeclarationByUri(const std::string& uri,
                                                   const Namespace** foundNamespace,
                                                   const Element** foundElement) const
{
    const Element* element = this;
    const Namespace* ns = nullptr;

    for (; element; element = element->getParent()) {
        ns = findNamespaceByUri(uri);
        if (ns)
            break;
    }

    if (foundNamespace)
        *foundNamespace = ns;
    if (foundElement)
        *foundElement = element;
}

void Element::findElementNamespaceDeclarationByUri(const std::string& uri,
                                                   Namespace** foundNamespace,
                                                   Element** foundElement)
{
    Element* element = this;
    Namespace* ns = nullptr;

    for (; element; element = element->getParent()) {
        ns = findNamespaceByUri(uri);
        if (ns)
            break;
    }

    if (foundNamespace)
        *foundNamespace = ns;
    if (foundElement)
        *foundElement = element;
}


void Element::findElementNamespaceDeclarationByPrefix(const std::string& prefix,
                                                      const Namespace** foundNamespace,
                                                      const Element** foundElement) const
{
    const Element* element = this;
    const Namespace* ns = nullptr;

    for (; element; element = element->getParent()) {
        ns = element->findNamespaceByPrefix(prefix);
        if (ns)
            break;
    }

    if (foundNamespace)
        *foundNamespace = ns;
    if (foundElement)
        *foundElement = element;
}

void Element::findElementNamespaceDeclarationByPrefix(const std::string& prefix,
                                                      Namespace** foundNamespace,
                                                      Element** foundElement)
{
    Element* element = this;
    Namespace* ns = nullptr;

    for (; element; element = element->getParent()) {
        ns = findNamespaceByPrefix(prefix);
        if (ns)
            break;
    }

    if (foundNamespace)
        *foundNamespace = ns;
    if (foundElement)
        *foundElement = element;
}


Namespace* Element::findNamespaceDeclarationByUri(const std::string& uri)
{
    Namespace* ns = nullptr;
    findElementNamespaceDeclarationByUri(uri, &ns, nullptr);
    return ns;
}

const Namespace* Element::findNamespaceDeclarationByUri(const std::string& uri) const
{
    const Namespace* ns = nullptr;
    findElementNamespaceDeclarationByUri(uri, &ns, nullptr);
    return ns;
}

Namespace* Element::findNamespaceDeclarationByPrefix(const std::string& prefix)
{
    Namespace* ns = nullptr;
    findElementNamespaceDeclarationByPrefix(prefix, &ns, nullptr);
    return ns;
}

const Namespace* Element::findNamespaceDeclarationByPrefix(const std::string& prefix) const
{
    const Namespace* ns = nullptr;
    findElementNamespaceDeclarationByPrefix(prefix, &ns, nullptr);
    return ns;
}


Element* Element::findElementByNamespaceDeclarationUri(const std::string& uri)
{
    Element* element = nullptr;
    findElementNamespaceDeclarationByUri(uri, nullptr, &element);
    return element;
}

const Element* Element::findElementByNamespaceDeclarationUri(const std::string& uri) const
{
    const Element* element = nullptr;
    findElementNamespaceDeclarationByUri(uri, nullptr, &element);
    return element;
}

Element* Element::findElementByNamespaceDeclarationPrefix(const std::string& prefix)
{
    Element* element = nullptr;
    findElementNamespaceDeclarationByPrefix(prefix, nullptr, &element);
    return element;
}

const Element* Element::findElementByNamespaceDeclarationPrefix(const std::string& prefix) const
{
    const Element* element = nullptr;
    findElementNamespaceDeclarationByPrefix(prefix, nullptr, &element);
    return element;
}


void Element::replacePrefix(const std::string& newPrefix, bool recursive)
{
    if (recursive) {
        for (Element* element = getFirstChildElement(); element;
             element = element->getNextSiblingElement()) {
            if (element->prefix == prefix) {
                element->replacePrefix(newPrefix, recursive);
            }
        }
    }

    prefix = newPrefix;
}

} // namespace xml
} // namespace ngrest

