#include <string.h>

#include "ObjectModel.h"

namespace ngrest {

NamedNode* Object::findChildByName(const char* name) const
{
    for (NamedNode* child = firstChild; child; child = child->nextSibling)
        if (!strcmp(child->name, name))
            return child;

    return nullptr;
}


}
