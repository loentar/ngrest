#include <string.h>

#include "Message.h"

namespace ngrest {

const Header* Request::getHeader(const char* name) const
{
    for (const Header* header = headers; header; header = header->next)
        if (!strcmp(name, header->name))
            return header;
    return nullptr;
}

}
