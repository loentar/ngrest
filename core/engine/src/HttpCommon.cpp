#include <string.h>

#include "HttpCommon.h"

namespace ngrest {

const HttpHeader* HttpRequest::getHeader(const char* name) const
{
    for (const HttpHeader* header = headers; header; header = header->next)
        if (!strcmp(name, header->name))
            return header;
    return nullptr;
}

}
