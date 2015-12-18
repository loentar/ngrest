#ifndef NGREST_MESSAGECONTEXT_H
#define NGREST_MESSAGECONTEXT_H

#include <stdint.h>

#include <ngrest/common/Callback.h>
#include <ngrest/utils/MemPool.h>

namespace ngrest {

class Exception;
class Transport;
struct Node;
struct MessageContext;

struct Header
{
    const char* name;
    const char* value;
    const Header* next;

    inline Header(const char* name_ = nullptr, const char* value_ = nullptr, const Header* next_ = nullptr):
        name(name_), value(value_), next(next_)
    {
    }
};


struct Request
{
    const char* path = nullptr;
    const Header* headers = nullptr;

    char* body = nullptr;
    uint64_t bodySize = 0;

    Node* node;

    // name must be in lower case
    const Header* getHeader(const char* name) const;
};

struct Response
{
    const Header* headers = nullptr;

    Node* node;

//    char* body = nullptr;
//    uint64_t bodySize = 0;
    MemPool poolBody;
};

class MessageCallback: public Callback<MessageContext*>
{
};

struct MessageContext
{
    Transport* transport;
    Request* request = nullptr;
    Response* response = nullptr;
    MessageCallback* callback = nullptr;
    MemPool pool;
};

} // namespace ngrest

#endif // NGREST_MESSAGECONTEXT_H

