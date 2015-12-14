#ifndef NGREST_JSONREADER_H
#define NGREST_JSONREADER_H

namespace ngrest {

class MemPool;

namespace json {

class Node;

class JsonReader {
public:
    static Node* read(char* buff, MemPool& memPool);
};

}
}

#endif
