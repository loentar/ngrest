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

#ifndef NGREST_XML_ELEMENT_H
#define NGREST_XML_ELEMENT_H

#include <string>
#include "Node.h"
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

class NGREST_XML_EXPORT Attribute;
class NGREST_XML_EXPORT Namespace;

/**
 * @brief XML Element
 */
class NGREST_XML_EXPORT Element: public Node
{
public:
    /**
     * @brief initializing constructor
     * @param parent parent node
     */
    Element(Element* parent = nullptr);

    /**
     * @brief initializing constructor
     * @param name element name
     * @param parent parent node
     */
    Element(const std::string& name, Element* parent = nullptr);

    /**
     * @brief initializing constructor
     * @param name element name
     * @param value element's text node value
     * @param parent parent node
     */
    Element(const std::string& name, const Value& value, Element* parent = nullptr);

    /**
     * @brief cloning constructor
     * @param element source element
     */
    Element(const Element& element);

    /**
     * @brief cloning operator
     * @param element source element
     * @return *this
     */
    Element& operator=(const Element& element);

    /**
     * @brief destructor
     */
    virtual ~Element();


    /**
     * @brief clone xml tree
     * @param recursive true clone whole xml tree, false clone this node only
     * @return cloned xml tree
     */
    Element* cloneElement(bool recursive = true) const;

    /**
     * @brief clone xml tree to this element from given element
     * @param element source element
     * @param recursive true clone whole xml tree, false clone this node only
     * @return *this
     */
    Element& cloneElement(const Element& element, bool recursive = true);

    /**
     * @brief get element name
     * @return const reference to element name
    */
    const std::string& getName() const;

    /**
     * @brief set element name
     * @param name element name if form [prefix:]element_name
     */
    void setName(const std::string& name);

    /**
     * @brief get prefix
     * @return prefix
     */
    const std::string& getPrefix() const;

    /**
     * @brief get node name with prefix
     * @return node name with prefix
     */
    std::string getPrefixName() const;


    /**
     * @brief get concatenated text of children values
     * @return text
    */
    virtual std::string GetChildrenText() const;

    /**
     * @brief get first Text child content node
     * simple method to access text child value
     * @return first child text, or empty string if no child text exists
    */
    virtual const Value& getValue() const override;

    /**
     * @brief get node value
     * @return node value
    */
    virtual const std::string& getTextValue() const override;

    /**
     * @brief set node value
     * simple method to access text child value
        keep only one text child and set text value for it
     * @param value new node value
    */
    virtual void setValue(const Value& value) override;

    /**
     * @brief is text null
     * @return true if no text children exists
     */
    bool isTextNull() const;

    /**
     * @brief remove all text children
     */
    void setTextIsNull();

    /**
     * @brief is element empty
     * @return true if element have no child nodes
     */
    bool isEmpty() const;

    /**
     * @brief is element have no child nodes or exactly one text child node
     * @return true if element is leaf
     */
    bool isLeaf() const;

    /**
     * @brief clear element for reuse
     * remove all children, attributes, namespaces.
     */
    void clear();

    // children management

    /**
     * @brief create and append child element
     * @return created element
     */
    Element& createElement();

    /**
     * @brief create and append child element
     * @param name element name
     * @return created element
     */
    Element& createElement(const std::string& name);

    /**
     * @brief create and append child element
     * @param name element name
     * @param value node value
     * @return created element
     */
    Element& createElement(const std::string& name, const Value& value);

    /**
     * @brief create new child element if no child element with given name exists
     * @param name element name
     * @return created or already existing element
     */
    Element& createElementOnce(const std::string& name);

    /**
     * @brief create and append child Comment
     * @return created comment node
     */
    Comment& createComment();

    /**
     * @brief create and append child Comment
     * @param value node value
     * @return created comment node
     */
    Comment& createComment(const Value& value);

    /**
     * @brief create and append child Text
     * @return created text node
     */
    Text& createText();

    /**
     * @brief create and append child (Cdata, Comment or Text)
     * @param value node value
     * @return created text node
     */
    Text& createText(const Value& value);

    /**
     * @brief create and append child Cdata type
     * @return created CDATA node
     */
    Cdata& createCdata();

    /**
     * @brief create and append child Cdata type
     * @param value node value
     * @return created CDATA node
     */
    Cdata& createCdata(const Value& value);


    /**
     * @brief insert child node before element
     * @param node node to insert
     * @param before existing child node to insert before
     * @return reference to appended child
     */
    Node& insertChildBefore(Node* node, Node* before);

    /**
     * @brief insert child node before element
     * @param node node to insert
     * @param before existing child node to insert before
     * @return reference to appended child
     */
    Element& insertChildBefore(Element* node, Node* before);

    /**
     * @brief insert child node before element
     * @param node node to insert
     * @param before existing child node to insert before
     * @return reference to appended child
     */
    Comment& insertChildBefore(Comment* node, Node* before);

    /**
     * @brief insert child node before element
     * @param node node to insert
     * @param before existing child node to insert before
     * @return reference to appended child
     */
    Text& insertChildBefore(Text* node, Node* before);

    /**
     * @brief insert child node before element
     * @param node node to insert
     * @param before existing child node to insert before
     * @return reference to appended child
     */
    Cdata& insertChildBefore(Cdata* node, Node* before);


    /**
     * @brief insert child node after element
     * @param node node to insert
     * @param after existing child node to insert after
     * @return reference to appended child
     */
    Node& insertChildAfter(Node* node, Node* after);

    /**
     * @brief insert child node after element
     * @param node node to insert
     * @param after existing child node to insert after
     * @return reference to appended child
     */
    Element& insertChildAfter(Element* node, Node* after);

    /**
     * @brief insert child node after element
     * @param node node to insert
     * @param after existing child node to insert after
     * @return reference to appended child
     */
    Comment& insertChildAfter(Comment* node, Node* after);

    /**
     * @brief insert child node after element
     * @param node node to insert
     * @param after existing child node to insert after
     * @return reference to appended child
     */
    Text& insertChildAfter(Text* node, Node* after);

    /**
     * @brief insert child node after element
     * @param node node to insert
     * @param after existing child node to insert after
     * @return reference to appended child
     */
    Cdata& insertChildAfter(Cdata* node, Node* after);


    /**
     * @brief append child node
     * @param node child node
     * @return reference to appended child
     */
    Node& appendChild(Node* node);

    /**
     * @brief append child element
     * @param node child element
     * @return reference to appended child
     */
    Element& appendChild(Element* node);

    /**
     * @brief append child node
     * @param node child node
     * @return reference to appended child
     */
    Comment& appendChild(Comment* node);

    /**
     * @brief append child node
     * @param node child node
     * @return reference to appended child
     */
    Text& appendChild(Text* node);

    /**
     * @brief append child node
     * @param node child node
     * @return reference to appended child
     */
    Cdata& appendChild(Cdata* node);


    /**
     * @brief remove and free child node
     * this function is eqivalent to "delete node"
     * @param node child node to remove
     */
    void removeChild(Node* node);


    /**
     * @brief get first child
     * @return first child or nullptr if there is no children
     */
    const Node* getFirstChild() const;

    /**
     * @brief get first child
     * @return first child or nullptr if there is no children
     */
    Node* getFirstChild();

    /**
     * @brief get last child
     * @return last child or nullptr if there is no children
     */
    const Node* getLastChild() const;

    /**
     * @brief get last child
     * @return last child or nullptr if there is no children
     */
    Node* getLastChild();


    /**
     * @brief get first child element
     * @return first child element or nullptr if there is no child elements
     */
    const Element* getFirstChildElement() const;

    /**
     * @brief get first child element
     * @return first child element or nullptr if there is no child elements
     */
    Element* getFirstChildElement();

    /**
     * @brief get last child element
     * @return last child element or nullptr if there is no child elements
     */
    const Element* getLastChildElement() const;

    /**
     * @brief get last child element
     * @return last child element or nullptr if there is no child elements
     */
    Element* getLastChildElement();


    /**
     * @brief find child element by name
     * @param name child element name
     * @return child element or nullptr if no child found
    */
    const Element* findChildElementByName(const std::string& name) const;

    /**
     * @brief find child element by name
     * @param name child element name
     * @return child element or nullptr if no child found
    */
    Element* findChildElementByName(const std::string& name);

    /**
     * @brief find child element by name
     * @param name child element name
     * @param begin begin search from
     * @return child element or nullptr if no child found
    */
    const Element* findChildElementByName(const std::string& name, const Element* begin) const;

    /**
     * @brief find child element by name
     * @param name child element name
     * @param begin begin search from
     * @return child element or nullptr if no child found
    */
    Element* findChildElementByName(const std::string& name, Element* begin);

    /**
     * @brief get child element by name
     * @throws an exception if no child found
     * @param name child element name
     * @return const reference to child element
    */
    const Element& getChildElementByName(const std::string& name) const;

    /**
     * @brief get child element by name
     * @throws an exception if no child found
     * @param name child element name
     * @return reference to child element
    */
    Element& getChildElementByName(const std::string& name);


    /**
     * @brief calculate the number of children
     * @return number of children
    */
    unsigned getChildrenCount() const;

    /**
     * @brief calculate the number of children elements
     * @return number of children elements
    */
    unsigned getChildrenElementsCount() const;


    // attributes management

    /**
     * @brief create attribute with name and value
     * @param name attribute name
     * @param value attribute value
     */
    Attribute& createAttribute(const std::string& name, const std::string& value);

    /**
     * @brief create attribute with name, value and prefix
     * @param name attribute name
     * @param value attribute value
     * @param prefix attribute prefix
     */
    Attribute& createAttribute(const std::string& name, const std::string& value,
                               const std::string& prefix);

    /**
     * @brief append attribute's copy
     * @param attribute attribute
     */
    Attribute& appendAttribute(const Attribute& attribute);

    /**
     * @brief append attribute
     * @param attribute attribute
     */
    Attribute& appendAttribute(Attribute* attribute);


    /**
     * @brief find attribute by name
     * @param name attribute name
     * @return const iterator to attribute
    */
    const Attribute* findAttribute(const std::string& name) const;

    /**
     * @brief find attribute by name
     * @param name attribute name
     * @return iterator to attribute
    */
    Attribute* findAttribute(const std::string& name);

    /**
     * @brief get attribute by name
     * @throws an exception if no attribute found
     * @param name attribute name
     * @return attribute value
    */
    const Attribute& getAttribute(const std::string& name) const;

    /**
     * @brief get attribute by name
     * @throws an exception if no attribute found
     * @param name attribute name
     * @return attribute value
    */
    Attribute& getAttribute(const std::string& name);

    /**
     * @brief get attribute value by name
     * @throws an exception if no attribute found
     * @param name attribute name
     * @return attribute value
    */
    const std::string& getAttributeValue(const std::string& name) const;

    /**
     * @brief get attribute value by name
     * @throws an exception if no attribute found
     * @param name attribute name
     * @param value attribute value
     * @return attribute value
    */
    void setAttributeValue(const std::string& name, const std::string& value);


    /**
     * @brief remove attribute
     * @param attribute attribute to remove
     */
    void removeAttribute(Attribute* attribute);

    /**
     * @brief remove attribute by name
     * @param name attribute name to remove
     */
    void removeAttributeByName(const std::string& name);

    /**
     * @brief is attribute exists
     * @param name attribute name
     * @return true, if attribute exists
    */
    bool isAttributeExists(const std::string& name) const;


    /**
     * @brief get first attribute
     * @return pointer to first attribute
     */
    const Attribute* getFirstAttribute() const;

    /**
     * @brief get first attribute
     * @return pointer to first attribute
     */
    Attribute* getFirstAttribute();


    // namespaces management

    /**
     * @brief declare default namespace
     * @param uri namespace uri
     */
    Namespace& declareDefaultNamespace(const std::string& uri);

    /**
     * @brief declare new namespace / modify existing
     * @param uri namespace uri
     * @param prefix namespace prefix
     */
    Namespace& declareNamespace(const std::string& uri, const std::string& prefix);

    /**
     * @brief declare new namespace (create clone) / modify existing
     * @param ns namespace
     * @return new namespace
     */
    Namespace& declareNamespace(const Namespace& ns);

    /**
     * @brief declare and set new namespace
     * @param uri namespace uri
     * @param prefix namespace prefix
     * @param recursive set namespace for children too
     * @return new namespace
     */
    Namespace& setNamespace(const std::string& uri, const std::string& prefix,
                            bool recursive = true);

    /**
     * @brief declare and set new namespace (copy)
     * @param ns namespace
     * @param recursive set namespace for children too
     * @return new namespace
     */
    Namespace& setNamespace(const Namespace& ns, bool recursive = true);


    /**
     * @brief get namespace of element
     * @return namespace of element
     */
    const Namespace* getNamespace() const;

    /**
     * @brief get namespace of element
     * @return namespace of element
     */
    Namespace* getNamespace();

    /**
     * @brief get namespace prefix by uri
     * @param uri uri
     * @return namespace prefix
     */
    const std::string& getNamespacePrefixByUri(const std::string& uri);

    /**
     * @brief get namespace uri by prefix
     * @param prefix prefix
     * @return namespace uri
     */
    const std::string& getNamespaceUriByPrefix(const std::string& prefix);

    /**
     * @brief find namespace by uri
     * @param uri namespace uri
     * @return namespace or nullptr, if no namespace found
     */
    const Namespace* findNamespaceByUri(const std::string& uri) const;

    /**
     * @brief find namespace by uri
     * @param uri namespace uri
     * @return namespace or nullptr, if no namespace found
     */
    Namespace* findNamespaceByUri(const std::string& uri);

    /**
     * @brief find namespace by prefix
     * @param prefix prefix
     * @return namespace or nullptr, if no namespace found
     */
    const Namespace* findNamespaceByPrefix(const std::string& prefix) const;

    /**
     * @brief find namespace by prefix
     * @param prefix prefix
     * @return namespace or nullptr, if no namespace found
     */
    Namespace* findNamespaceByPrefix(const std::string& prefix);

    /**
     * @brief get first namespace
     * @return pointer to first namespace
     */
    const Namespace* getFirstNamespace() const;

    /**
     * @brief get first declared namespace
     * @return pointer to first declared namespace
     */
    Namespace* getFirstNamespace();


    /**
     * @brief find element namespace declaration by uri
     * start to find from current element and go up the hierarchy
     * @param uri uri
     * @param foundNamespace (optional) found namespace or nullptr
     * @param foundElement (optional) found element or nullptr
     */
    void findElementNamespaceDeclarationByUri(const std::string& uri,
                                              const Namespace** foundNamespace,
                                              const Element** foundElement) const;

    /**
     * @brief find element namespace declaration by uri
     * start to find from current element and go up the hierarchy
     * @param uri uri
     * @param foundNamespace (optional) found namespace or nullptr
     * @param foundElement (optional) found element or nullptr
     */
    void findElementNamespaceDeclarationByUri(const std::string& uri,
                                              Namespace** foundNamespace,
                                              Element** foundElement);

    /**
     * @brief find element namespace declaration by prefix
     * start to find from current element and go up the hierarchy
     * @param prefix prefix
     * @param foundNamespace (optional) found namespace or nullptr
     * @param foundElement (optional) found element or nullptr
     */
    void findElementNamespaceDeclarationByPrefix(const std::string& prefix,
                                                 const Namespace** foundNamespace,
                                                 const Element** foundElement) const;

    /**
     * @brief find element namespace declaration by prefix
     * start to find from current element and go up the hierarchy
     * @param prefix prefix
     * @param foundNamespace (optional) found namespace or nullptr
     * @param foundElement (optional) found element or nullptr
     */
    void findElementNamespaceDeclarationByPrefix(const std::string& prefix,
                                                 Namespace** foundNamespace,
                                                 Element** foundElement);


    /**
     * @brief find namespace declaration
     * convinent wrapper for findElementNamespaceDeclarationByUri
     * @param uri namespace uri to find
     * @return found namespace or nullptr
     */
    const Namespace* findNamespaceDeclarationByUri(const std::string& uri) const;

    /**
     * @brief find namespace declaration
     * convinent wrapper for findElementNamespaceDeclarationByUri
     * @param uri namespace uri to find
     * @return found namespace or nullptr
     */
    Namespace* findNamespaceDeclarationByUri(const std::string& uri);

    /**
     * @brief find element where given namespace is declared
     * convinent wrapper for findElementNamespaceDeclarationByPrefix
     * @param prefix prefix
     * @return element where this namespace is declared
     */
    const Namespace* findNamespaceDeclarationByPrefix(const std::string& prefix) const;

    /**
     * @brief find element where given namespace is declared
     * convinent wrapper for findElementNamespaceDeclarationByPrefix
     * @param prefix prefix
     * @return element where this namespace is declared
     */
    Namespace* findNamespaceDeclarationByPrefix(const std::string& prefix);


    /**
     * @brief find element where given namespace is declared
     * convinent wrapper for findElementNamespaceDeclarationByUri
     * @param uri uri
     * @return element where this namespace is declared
     */
    const Element* findElementByNamespaceDeclarationUri(const std::string& uri) const;

    /**
     * @brief find element where given namespace is declared
     * convinent wrapper for findElementNamespaceDeclarationByUri
     * @param uri uri
     * @return element where this namespace is declared
     */
    Element* findElementByNamespaceDeclarationUri(const std::string& uri);

    /**
     * @brief find element where given namespace is declared
     * convinent wrapper for findElementNamespaceDeclarationByPrefix
     * @param prefix prefix
     * @return element where this namespace is declared
     */
    const Element* findElementByNamespaceDeclarationPrefix(const std::string& prefix) const;

    /**
     * @brief find element where given namespace is declared
     * convinent wrapper for findElementNamespaceDeclarationByPrefix
     * @param prefix prefix
     * @return element where this namespace is declared
     */
    Element* findElementByNamespaceDeclarationPrefix(const std::string& prefix);

private:
    void replacePrefix(const std::string& newPrefix, bool recursive);

private:
    std::string     name;            //!< element name
    Attribute*      firstAttribute;  //!< first attribute
    Namespace*      firstNamespace;  //!< first namespace
    std::string     prefix;          //!< namespace prefix of element
    Node*           firstChild;      //!< first child
    Node*           lastChild;       //!< last child
    friend class Node;
};

} // namespace xml
} // namespace ngrest

#endif // #ifndef NGREST_XML_ELEMENT_H
