#include "Namespace.h"

namespace ngrest {
namespace xml {

Namespace::Namespace(const Namespace& ns):
    prefix(ns.prefix),
    uri(ns.uri),
    nextSibling(nullptr)
{
}

Namespace::Namespace(const std::string& prefix_, const std::string& uri_):
    prefix(prefix_),
    uri(uri_),
    nextSibling(nullptr)
{
}

Namespace& Namespace::operator=(const Namespace& ns)
{
    prefix = ns.prefix;
    uri = ns.uri;
    return *this;
}


const std::string& Namespace::getPrefix() const
{
    return prefix;
}

void Namespace::setPrefix(const std::string& prefix)
{
    this->prefix = prefix;
}


const std::string& Namespace::getUri() const
{
    return uri;
}

void Namespace::setUri(const std::string& uri)
{
    this->uri = uri;
}

const Namespace* Namespace::getNextSibling() const
{
    return nextSibling;
}

Namespace* Namespace::getNextSibling()
{
    return nextSibling;
}


bool Namespace::operator==(const Namespace& ns) const
{
    return prefix == ns.prefix && uri == ns.uri;
}

bool Namespace::operator!=(const Namespace& ns) const
{
    return !(operator==(ns));
}

} // namespace xml
} // namespace ngrest

