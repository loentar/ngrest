#ifndef NGREST_JSONWRITER_H
#define NGREST_JSONWRITER_H

namespace ngrest {

class MemPool;

namespace json {

class Node;

class JsonWriter {
public:
    static void write(const Node* node, MemPool& memPool, int indent = 0);
};

}
}

#endif
