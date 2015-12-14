#include <string.h>

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/MemPool.h>

#include "JsonTypes.h"
#include "JsonReader.h"

namespace ngrest {
namespace json {

class JsonReaderImpl {
public:
    int line = 1;
    char* begin;
    char* curr;
    MemPool& pool;

    inline void skipWs()
    {
        for (;; ++curr) {
            if (*curr == '\n') {
                ++line;
            } else if (*curr != ' ' && *curr != '\r' && *curr != '\t') {
                break;
            }
        }
    }

    inline bool seekTo(char ch)
    {
        for (; *curr != '\0'; ++curr)
            if (*curr == ch)
                return true;
        return false;
    }

    inline char* tokenValue() {
        char* start = curr;
        while (*curr != '}' &&
               *curr != ']' &&
               *curr != ',' &&
               *curr != ' ' &&
               *curr != '\r' &&
               *curr != '\t') {
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading token");

            if (*curr == '\n') {
                ++line;
                break;
            }
            ++curr;
        }
        // avoid parsing error. terminate token within readObject or readArray
        // *curr = '\0';
        // ++curr;
        return start;
    }

    inline char* tokenString() {
        ++curr; // skip '"'
        char* start = curr;
        while (*curr != '"') {
            if (*curr == '\\') { // skip '\"'
                ++curr;
                if (*curr == '"')
                    ++curr;
            }
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading token");

            if (*curr == '\n') {
                ++line;
                break;
            }
            ++curr;
        }
        *curr = '\0';
        ++curr;
        return start;
    }


    inline JsonReaderImpl(char* buff, MemPool& memPool):
        begin(buff),
        curr(buff),
        pool(memPool)
    {
    }

    inline Node* readAny()
    {
        skipWs();

        switch (*curr) {
        case '[':
            return readArray();

        case '{':
            return readObject();

        case '"': // string
            return pool.alloc<Value>(ValueTypeString, tokenString());
        }

        // number or special value
        const char* token = tokenValue();
        const int len = curr - token;

        // number
        if ((*token >= '0' && *token <= '9') || *token == '-')
            return pool.alloc<Value>(ValueTypeNumber, token);

        if (!strncmp(token, "true", len) || !strncmp(token, "false", len))
            return pool.alloc<Value>(ValueTypeBoolean, token);

        // handle undefined, NaN, null
        if (!strncmp(token, "null", len))
            return pool.alloc<Value>(ValueTypeNull);

        if (!strncmp(token, "undefined", len))
            return pool.alloc<Value>(ValueTypeUndefined);

        if (!strncmp(token, "NaN", len))
            return pool.alloc<Value>(ValueTypeNaN);

        NGREST_THROW_ASSERT("Unexpected token");
    }

    inline Node* readArrayOrObject()
    {
        skipWs();

        switch (*curr) {
        case '[':
            return readArray();
        case '{':
            return readObject();
        }

        NGREST_THROW_ASSERT("Unexpected symbol");
    }

    inline Array* readArray()
    {
        ++curr; // skip '['

        Array* array = pool.alloc<Array>();

        skipWs();
        // empty array
        if (*curr == ']') {
            ++curr;
            return array;
        }

        LinkedNode* prevLinkedNode = nullptr;
        LinkedNode* linkedNode;
        char* valueEnd;

        for (;;) {
            linkedNode = pool.alloc<LinkedNode>(readAny());

            if (prevLinkedNode == nullptr) {
                array->firstChild = linkedNode;
            } else {
                prevLinkedNode->nextSibling = linkedNode;
            }
            prevLinkedNode = linkedNode;

            valueEnd = (linkedNode->node->type == TypeValue) ? curr : nullptr;
            skipWs();
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading array");

            if (*curr == ']') {
                // terminate token
                if (valueEnd != nullptr)
                    *valueEnd = '\0';
                ++curr;
                return array;
            }
            NGREST_ASSERT(*curr == ',', "Missing ',' while reading array");
            if (valueEnd != nullptr)
                *valueEnd = '\0';
            ++curr;
        }
    }

    inline Object* readObject()
    {
        // {"name": ...., "name2":...}
        ++curr; // skip '{'

        Object* object = pool.alloc<Object>();

        skipWs();
        // empty object
        if (*curr == '}') {
            ++curr;
            return object;
        }

        NamedNode* prevNamedNode = nullptr;
        NamedNode* namedNode;
        char* valueEnd;

        for (;;) {
            NGREST_ASSERT(*curr == '"', "Missing '\"' while reading object name");
            namedNode = pool.alloc<NamedNode>(tokenString());

            skipWs();
            NGREST_ASSERT(*curr == ':', "Missing ':' after object name");
            ++curr;

            namedNode->node = readAny();

            if (prevNamedNode == nullptr) {
                object->firstChild = namedNode;
            } else {
                prevNamedNode->nextSibling = namedNode;
            }
            prevNamedNode = namedNode;

            valueEnd = (namedNode->node->type == TypeValue) ? curr : nullptr;
            skipWs();
            NGREST_ASSERT(*curr != '\0', "Unexpected EOF while reading object");

            if (*curr == '}') {
                if (valueEnd != nullptr)
                    *valueEnd = '\0';
                ++curr;
                return object;
            }
            NGREST_ASSERT(*curr == ',', "Missing ',' while reading object");
            if (valueEnd != nullptr)
                *valueEnd = '\0';
            ++curr;
            skipWs();
        }
    }
};

Node* JsonReader::read(char* buff, MemPool& memPool)
{
    return JsonReaderImpl(buff, memPool).readArrayOrObject();
}


}
}
