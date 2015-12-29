#include "Exception.h"
#include "Document.h"

namespace ngrest {
namespace xml {

const Declaration& Document::getDeclaration() const
{
    return declaration;
}

Declaration& Document::getDeclaration()
{
    return declaration;
}

const Element& Document::getRootElement() const
{
    return rootElement;
}

Element& Document::getRootElement()
{
    return rootElement;
}

} // namespace xml
} // namespace ngrest
