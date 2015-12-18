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
