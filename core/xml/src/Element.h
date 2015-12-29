#ifndef NGREST_XML_ELEMENT_H
#define NGREST_XML_ELEMENT_H

#include <string>
#include "Node.h"
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

class NGREST_XML_EXPORT Attribute;
class NGREST_XML_EXPORT Namespace;

//! XML Element
class NGREST_XML_EXPORT Element: public Node
{
public:
    //! initializing constructor
    /*! \param  parent - parent node
        */
    Element(Element* parent = nullptr);

    //! initializing constructor
    /*! \param  name - element name
        \param  parent - parent node
        */
    Element(const std::string& name, Element* parent = nullptr);

    //! initializing constructor
    /*! \param  name - element name
        \param  value - element's text node value
        \param  parent - parent node
        */
    Element(const std::string& name, const Value& value, Element* parent = nullptr);

    //! cloning constructor
    /*! \param  element - source element
      */
    Element(const Element& element);

    //! cloning operator
    /*! \param  element - source element
        \return *this
      */
    Element& operator=(const Element& element);

    //! destructor
    virtual ~Element();


    //! clone xml tree
    /*! \param  recursive - true - clone whole xml tree, false - clone this node only
        \return cloned xml tree
      */
    Element* cloneElement(bool recursive = true) const;

    //! clone xml tree to this element from given element
    /*! \param  element - source element
        \param  recursive - true - clone whole xml tree, false - clone this node only
        \return *this
      */
    Element& cloneElement(const Element& element, bool recursive = true);

    //! get element name
    /*! \return const reference to element name
    */
    const std::string& getName() const;

    //! set element name
    /*! \param name - element name if form [prefix:]element_name
      */
    void setName(const std::string& name);

    //! get prefix
    /*! \return prefix
      */
    const std::string& getPrefix() const;

    //! get node name with prefix
    /*! \return node name with prefix
      */
    std::string getPrefixName() const;


    //! get concatenated text of children values
    /*! \return text
    */
    virtual std::string GetChildrenText() const;

    //! get first Text child content node
    /*! simple method to access text child value
        \return first child text, or empty string if no child text exists
    */
    virtual const Value& getValue() const override;

    //! get node value
    /*! \return node value
    */
    virtual const std::string& getTextValue() const override;

    //! set node value
    /*! simple method to access text child value
        keep only one text child and set text value for it
        \param value - new node value
    */
    virtual void setValue(const Value& value) override;

    //! is text null
    /*! \return true if no text children exists
      */
    bool isTextNull() const;

    //! remove all text children
    void setTextIsNull();

    //! is element empty
    /*! \return true if element have no child nodes
      */
    bool isEmpty() const;

    //! is element have no child nodes or exactly one text child node
    /*! \return true if element is leaf
      */
    bool isLeaf() const;

    //! clear element for reuse
    /*! remove all children, attributes, namespaces.
      */
    void clear();

    // children management

    //! create and append child element
    /*! \return created element
      */
    Element& createElement();

    //! create and append child element
    /*! \param name - element name
        \return created element
      */
    Element& createElement(const std::string& name);

    //! create and append child element
    /*! \param name - element name
        \param value - node value
        \return created element
      */
    Element& createElement(const std::string& name, const Value& value);

    //! create new child element if no child element with given name exists
    /*! \param name - element name
        \return created or already existing element
      */
    Element& createElementOnce(const std::string& name);

    //! create and append child Comment
    /*! \return created comment node
      */
    Comment& createComment();

    //! create and append child Comment
    /*! \param value - node value
        \return created comment node
      */
    Comment& createComment(const Value& value);

    //! create and append child Text
    /*! \return created text node
      */
    Text& createText();

    //! create and append child (Cdata, Comment or Text)
    /*! \param value - node value
        \return created text node
      */
    Text& createText(const Value& value);

    //! create and append child Cdata type
    /*! \return created CDATA node
      */
    Cdata& createCdata();

    //! create and append child Cdata type
    /*! \param value - node value
        \return created CDATA node
      */
    Cdata& createCdata(const Value& value);


    //! insert child node before element
    /*! \param node - node to insert
        \param before - existing child node to insert before
        \return reference to appended child
      */
    Node& insertChildBefore(Node* node, Node* before);

    //! insert child node before element
    /*! \param node - node to insert
        \param before - existing child node to insert before
        \return reference to appended child
      */
    Element& insertChildBefore(Element* node, Node* before);

    //! insert child node before element
    /*! \param node - node to insert
        \param before - existing child node to insert before
        \return reference to appended child
      */
    Comment& insertChildBefore(Comment* node, Node* before);

    //! insert child node before element
    /*! \param node - node to insert
        \param before - existing child node to insert before
        \return reference to appended child
      */
    Text& insertChildBefore(Text* node, Node* before);

    //! insert child node before element
    /*! \param node - node to insert
        \param before - existing child node to insert before
        \return reference to appended child
      */
    Cdata& insertChildBefore(Cdata* node, Node* before);


    //! insert child node after element
    /*! \param node - node to insert
        \param after - existing child node to insert after
        \return reference to appended child
      */
    Node& insertChildAfter(Node* node, Node* after);

    //! insert child node after element
    /*! \param node - node to insert
        \param after - existing child node to insert after
        \return reference to appended child
      */
    Element& insertChildAfter(Element* node, Node* after);

    //! insert child node after element
    /*! \param node - node to insert
        \param after - existing child node to insert after
        \return reference to appended child
      */
    Comment& insertChildAfter(Comment* node, Node* after);

    //! insert child node after element
    /*! \param node - node to insert
        \param after - existing child node to insert after
        \return reference to appended child
      */
    Text& insertChildAfter(Text* node, Node* after);

    //! insert child node after element
    /*! \param node - node to insert
        \param after - existing child node to insert after
        \return reference to appended child
      */
    Cdata& insertChildAfter(Cdata* node, Node* after);


    //! append child node
    /*! \param node - child node
        \return reference to appended child
      */
    Node& appendChild(Node* node);

    //! append child element
    /*! \param node - child element
        \return reference to appended child
      */
    Element& appendChild(Element* node);

    //! append child node
    /*! \param node - child node
        \return reference to appended child
      */
    Comment& appendChild(Comment* node);

    //! append child node
    /*! \param node - child node
        \return reference to appended child
      */
    Text& appendChild(Text* node);

    //! append child node
    /*! \param node - child node
        \return reference to appended child
      */
    Cdata& appendChild(Cdata* node);


    //! remove and free child node
    /*! this function is eqivalent to "delete node"
        \param node - child node to remove
      */
    void removeChild(Node* node);


    //! get first child
    /*! \return first child or nullptr if there is no children
      */
    const Node* getFirstChild() const;

    //! get first child
    /*! \return first child or nullptr if there is no children
      */
    Node* getFirstChild();

    //! get last child
    /*! \return last child or nullptr if there is no children
      */
    const Node* getLastChild() const;

    //! get last child
    /*! \return last child or nullptr if there is no children
      */
    Node* getLastChild();


    //! get first child element
    /*! \return first child element or nullptr if there is no child elements
      */
    const Element* getFirstChildElement() const;

    //! get first child element
    /*! \return first child element or nullptr if there is no child elements
      */
    Element* getFirstChildElement();

    //! get last child element
    /*! \return last child element or nullptr if there is no child elements
      */
    const Element* getLastChildElement() const;

    //! get last child element
    /*! \return last child element or nullptr if there is no child elements
      */
    Element* getLastChildElement();


    //! find child element by name
    /*! \param  name - child element name
        \return child element or nullptr if no child found
    */
    const Element* findChildElementByName(const std::string& name) const;

    //! find child element by name
    /*! \param  name - child element name
        \return child element or nullptr if no child found
    */
    Element* findChildElementByName(const std::string& name);

    //! find child element by name
    /*! \param  name - child element name
        \param  begin - begin search from
        \return child element or nullptr if no child found
    */
    const Element* findChildElementByName(const std::string& name, const Element* begin) const;

    //! find child element by name
    /*! \param  name - child element name
        \param  begin - begin search from
        \return child element or nullptr if no child found
    */
    Element* findChildElementByName(const std::string& name, Element* begin);

    //! get child element by name
    /*! throws an exception if no child found
        \param  name - child element name
        \return const reference to child element
    */
    const Element& getChildElementByName(const std::string& name) const;

    //! get child element by name
    /*! throws an exception if no child found
        \param  name - child element name
        \return reference to child element
    */
    Element& getChildElementByName(const std::string& name);


    //! calculate the number of children
    /*! \return number of children
    */
    unsigned getChildrenCount() const;

    //! calculate the number of children elements
    /*! \return number of children elements
    */
    unsigned getChildrenElementsCount() const;


    // attributes management

    //! create attribute with name and value
    /*! \param  name - attribute name
        \param  value - attribute value
        */
    Attribute& createAttribute(const std::string& name, const std::string& value);

    //! create attribute with name, value and prefix
    /*! \param  name - attribute name
        \param  value - attribute value
        \param  prefix - attribute prefix
        */
    Attribute& createAttribute(const std::string& name, const std::string& value,
                               const std::string& prefix);

    //! append attribute's copy
    /*! \param  attribute - attribute
        */
    Attribute& appendAttribute(const Attribute& attribute);

    //! append attribute
    /*! \param  attribute - attribute
        */
    Attribute& appendAttribute(Attribute* attribute);


    //! find attribute by name
    /*! \param  name - attribute name
        \return const iterator to attribute
    */
    const Attribute* findAttribute(const std::string& name) const;

    //! find attribute by name
    /*! \param  name - attribute name
        \return iterator to attribute
    */
    Attribute* findAttribute(const std::string& name);

    //! get attribute by name
    /*! throws an exception if no attribute found
        \param  name - attribute name
        \return attribute value
    */
    const Attribute& getAttribute(const std::string& name) const;

    //! get attribute by name
    /*! throws an exception if no attribute found
        \param  name - attribute name
        \return attribute value
    */
    Attribute& getAttribute(const std::string& name);

    //! get attribute value by name
    /*! throws an exception if no attribute found
        \param  name - attribute name
        \return attribute value
    */
    const std::string& getAttributeValue(const std::string& name) const;

    //! get attribute value by name
    /*! throws an exception if no attribute found
        \param  name - attribute name
        \param  value - attribute value
        \return attribute value
    */
    void setAttributeValue(const std::string& name, const std::string& value);


    //! remove attribute
    /*! \param attribute - attribute to remove
      */
    void removeAttribute(Attribute* attribute);

    //! remove attribute by name
    /*! \param name - attribute name to remove
      */
    void removeAttributeByName(const std::string& name);

    //! is attribute exists
    /*! \param  name - attribute name
        \return true, if attribute exists
    */
    bool isAttributeExists(const std::string& name) const;


    //! get first attribute
    /*! \return pointer to first attribute
      */
    const Attribute* getFirstAttribute() const;

    //! get first attribute
    /*! \return pointer to first attribute
      */
    Attribute* getFirstAttribute();


    // namespaces management

    //! declare default namespace
    /*! \param  uri - namespace uri
        */
    Namespace& declareDefaultNamespace(const std::string& uri);

    //! declare new namespace / modify existing
    /*! \param  uri - namespace uri
        \param  prefix - namespace prefix
        */
    Namespace& declareNamespace(const std::string& uri, const std::string& prefix);

    //! declare new namespace (create clone) / modify existing
    /*! \param  ns - namespace
        \return new namespace
        */
    Namespace& declareNamespace(const Namespace& ns);

    //! declare and set new namespace
    /*! \param  uri - namespace uri
        \param  prefix - namespace prefix
        \param  recursive - set namespace for children too
        \return new namespace
        */
    Namespace& setNamespace(const std::string& uri, const std::string& prefix,
                            bool recursive = true);

    //! declare and set new namespace (copy)
    /*! \param  ns - namespace
        \param  recursive - set namespace for children too
        \return new namespace
        */
    Namespace& setNamespace(const Namespace& ns, bool recursive = true);


    //! get namespace of element
    /*! \return namespace of element
        */
    const Namespace* getNamespace() const;

    //! get namespace of element
    /*! \return namespace of element
        */
    Namespace* getNamespace();

    //! get namespace prefix by uri
    /*! \param uri - uri
        \return namespace prefix
      */
    const std::string& getNamespacePrefixByUri(const std::string& uri);

    //! get namespace uri by prefix
    /*! \param prefix - prefix
        \return namespace uri
      */
    const std::string& getNamespaceUriByPrefix(const std::string& prefix);

    //! find namespace by uri
    /*! \param uri - namespace uri
        \return namespace or nullptr, if no namespace found
      */
    const Namespace* findNamespaceByUri(const std::string& uri) const;

    //! find namespace by uri
    /*! \param uri - namespace uri
        \return namespace or nullptr, if no namespace found
      */
    Namespace* findNamespaceByUri(const std::string& uri);

    //! find namespace by prefix
    /*! \param prefix - prefix
        \return namespace or nullptr, if no namespace found
      */
    const Namespace* findNamespaceByPrefix(const std::string& prefix) const;

    //! find namespace by prefix
    /*! \param prefix - prefix
        \return namespace or nullptr, if no namespace found
      */
    Namespace* findNamespaceByPrefix(const std::string& prefix);

    //! get first namespace
    /*! \return pointer to first namespace
      */
    const Namespace* getFirstNamespace() const;

    //! get first declared namespace
    /*! \return pointer to first declared namespace
      */
    Namespace* getFirstNamespace();


    //! find element namespace declaration by uri
    /*! start to find from current element and go up the hierarchy
        \param uri - uri
        \param foundNamespace - (optional) found namespace or nullptr
        \param foundElement - (optional) found element or nullptr
      */
    void findElementNamespaceDeclarationByUri(const std::string& uri,
                                              const Namespace** foundNamespace,
                                              const Element** foundElement) const;

    //! find element namespace declaration by uri
    /*! start to find from current element and go up the hierarchy
        \param uri - uri
        \param foundNamespace - (optional) found namespace or nullptr
        \param foundElement - (optional) found element or nullptr
      */
    void findElementNamespaceDeclarationByUri(const std::string& uri,
                                              Namespace** foundNamespace,
                                              Element** foundElement);

    //! find element namespace declaration by prefix
    /*! start to find from current element and go up the hierarchy
        \param prefix - prefix
        \param foundNamespace - (optional) found namespace or nullptr
        \param foundElement - (optional) found element or nullptr
      */
    void findElementNamespaceDeclarationByPrefix(const std::string& prefix,
                                                 const Namespace** foundNamespace,
                                                 const Element** foundElement) const;

    //! find element namespace declaration by prefix
    /*! start to find from current element and go up the hierarchy
        \param prefix - prefix
        \param foundNamespace - (optional) found namespace or nullptr
        \param foundElement - (optional) found element or nullptr
      */
    void findElementNamespaceDeclarationByPrefix(const std::string& prefix,
                                                 Namespace** foundNamespace,
                                                 Element** foundElement);


    //! find namespace declaration
    /*! convinent wrapper for findElementNamespaceDeclarationByUri
        \param  uri - namespace uri to find
        \return found namespace or nullptr
        */
    const Namespace* findNamespaceDeclarationByUri(const std::string& uri) const;

    //! find namespace declaration
    /*! convinent wrapper for findElementNamespaceDeclarationByUri
        \param  uri - namespace uri to find
        \return found namespace or nullptr
        */
    Namespace* findNamespaceDeclarationByUri(const std::string& uri);

    //! find element where given namespace is declared
    /*! convinent wrapper for findElementNamespaceDeclarationByPrefix
        \param  prefix - prefix
        \return element where this namespace is declared
        */
    const Namespace* findNamespaceDeclarationByPrefix(const std::string& prefix) const;

    //! find element where given namespace is declared
    /*! convinent wrapper for findElementNamespaceDeclarationByPrefix
        \param  prefix - prefix
        \return element where this namespace is declared
        */
    Namespace* findNamespaceDeclarationByPrefix(const std::string& prefix);


    //! find element where given namespace is declared
    /*! convinent wrapper for findElementNamespaceDeclarationByUri
        \param  uri - uri
        \return element where this namespace is declared
        */
    const Element* findElementByNamespaceDeclarationUri(const std::string& uri) const;

    //! find element where given namespace is declared
    /*! convinent wrapper for findElementNamespaceDeclarationByUri
        \param  uri - uri
        \return element where this namespace is declared
        */
    Element* findElementByNamespaceDeclarationUri(const std::string& uri);

    //! find element where given namespace is declared
    /*! convinent wrapper for findElementNamespaceDeclarationByPrefix
        \param  prefix - prefix
        \return element where this namespace is declared
        */
    const Element* findElementByNamespaceDeclarationPrefix(const std::string& prefix) const;

    //! find element where given namespace is declared
    /*! convinent wrapper for findElementNamespaceDeclarationByPrefix
        \param  prefix - prefix
        \return element where this namespace is declared
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
