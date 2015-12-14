#ifndef JSONTYPES_H
#define JSONTYPES_H

namespace ngrest {
namespace json {

enum JsonType
{
    TypeNamedNode,
    TypeLinkedNode,
    TypeObject,
    TypeArray,
    TypeValue
};

enum JsonValueType
{
    ValueTypeUndefined,
    ValueTypeNull,
    ValueTypeNaN,
    ValueTypeString,
    ValueTypeNumber,
    ValueTypeBoolean
};

struct Node
{
    JsonType type;
//    Node* nextSibling = nullptr;
//    Node* firstChild = nullptr;
//    Node* parent = nullptr;

    inline Node(JsonType type_):
        type(type_)
    {
    }
};

struct LinkedNode: public Node {
    Node* node;
    LinkedNode* nextSibling = nullptr;

    inline LinkedNode(Node* node_ = nullptr):
        Node(TypeNamedNode),
        node(node_)
    {
    }
};

struct NamedNode: public Node {
    const char* name;
    Node* node;
    NamedNode* nextSibling = nullptr;

    inline NamedNode(const char* name_ = nullptr, Node* node_ = nullptr):
        Node(TypeNamedNode),
        name(name_),
        node(node_)
    {
    }
};

struct Object: public Node
{
    NamedNode* firstChild = nullptr;

    inline Object():
        Node(TypeObject)
    {
    }
};

struct Array: public Node
{
    LinkedNode* firstChild = nullptr;

    inline Array():
        Node(TypeArray)
    {
    }
};

struct Value: public Node
{
//    static constexpr const char* const NaN = "NaN";
//    static constexpr const char* const undefined = "undefined";

    JsonValueType valueType;
    const char* value;

//    inline Value(const char* value_ = nullptr):
//        Node(TypeValue),
//        valueType(ValueTypeString),
//        value(value_)
//    {
//    }

    inline Value(JsonValueType valueType_, const char* value_ = nullptr):
        Node(TypeValue),
        valueType(valueType_),
        value(value_)
    {
    }
};


}
}


#endif // JSONTYPES_H
