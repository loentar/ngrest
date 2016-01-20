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
        const NamedNode* namedNode = getNamedChild(object, name, NodeType::Value);
        const char* valueStr = static_cast<const Value*>(namedNode->node)->value;
        NGREST_ASSERT(valueStr, "Failed to read parameter " + std::string(name) + " is null");
        value = valueStr;
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
        NGREST_ASSERT(node, "Failed to get value: value is null");
        NGREST_ASSERT(node->type == NodeType::Value,
                      "Failed to get value: node type does not match");
        const char* valueStr = static_cast<const Value*>(node)->value;
        NGREST_ASSERT_NULL(valueStr);
        value = valueStr;
    }
};

} // namespace ngrest

#endif // NGREST_OBJECTMODELUTILS_H
