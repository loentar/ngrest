#include <string.h>

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/MemPool.h>

#include <ngrest/common/ObjectModel.h>

#include "JsonWriter.h"

namespace ngrest {
namespace json {

class JsonWriterImpl {
public:
    MemPool& pool;
    int indent;

    inline JsonWriterImpl(MemPool& memPool, int indent_):
        pool(memPool),
        indent(indent_)
    {
    }

    inline void writeNode(const Node* node)
    {
        switch (node->type) {

        case NodeType::Object: {
            const Object* object = static_cast<const Object*>(node);
            pool.putChar('{');
            for (const NamedNode* child = object->firstChild; child; child = child->nextSibling) {
                if (child != object->firstChild)
                    pool.putChar(',');
                pool.putChar('"');
                pool.putCString(child->name);
                pool.putChar('"');
                pool.putChar(':');
                writeNode(child->node);
            }
            pool.putChar('}');
            break;
        }

        case NodeType::Array: {
            const Array* array = static_cast<const Array*>(node);
            pool.putChar('[');
            for (const LinkedNode* child = array->firstChild; child; child = child->nextSibling) {
                if (child != array->firstChild)
                    pool.putChar(',');
                writeNode(child->node);
            }
            pool.putChar(']');
            break;
        }

        case NodeType::Value: {
            const Value* value = static_cast<const Value*>(node);
            switch (value->valueType) {
            case ValueType::Undefined:
                pool.putData("undefined", 9);
                break;

            case ValueType::Null:
                pool.putData("null", 4);
                break;

            case ValueType::NaN:
                pool.putData("NaN", 3);
                break;

            case ValueType::String:
                pool.putChar('"');
                pool.putCString(value->value);
                pool.putChar('"');
                break;

            case ValueType::Number:
            case ValueType::Boolean:
                pool.putCString(value->value);
                break;
            }
            break;
        }
        }
    }
};

void JsonWriter::write(const Node* node, MemPool& memPool, int indent)
{
    NGREST_ASSERT(memPool.isClean(), "Mempool must be clean!");

    JsonWriterImpl(memPool, indent).writeNode(node);
}


}
}
