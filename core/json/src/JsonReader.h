#ifndef NGREST_JSONREADER_H
#define NGREST_JSONREADER_H

namespace ngrest {

class MemPool;
class Node;

namespace json {

class JsonReader {
public:
    static Node* read(char* buff, MemPool& memPool);
};

}
}

#endif
