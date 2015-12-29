#include "Exception.h"
#include "Attribute.h"

namespace ngrest {
namespace xml {

Attribute::Attribute(const Attribute& attr):
    prefix(attr.prefix),
    name(attr.name),
    value(attr.value),
    nextSibling(nullptr)
{
}

Attribute::Attribute(const std::string& name):
    nextSibling(nullptr)
{
    setName(name);
}

Attribute::Attribute(const std::string& name, const std::string& value_):
    value(value_),
    nextSibling(nullptr)
{
    setName(name);
}

Attribute::Attribute(const std::string& name_, const std::string& value_, const std::string& prefix_):
    prefix(prefix_),
    name(name_),
    value(value_),
    nextSibling(nullptr)
{
}

const std::string& Attribute::getPrefix() const
{
    return prefix;
}

void Attribute::setPrefix(const std::string& prefix)
{
    this->prefix = prefix;
}

const std::string& Attribute::getName() const
{
    return name;
}

void Attribute::setName(const std::string& name)
{
    std::string::size_type pos = name.find_last_of(':');
    if (pos == std::string::npos) {
        this->name = name;
        prefix.erase();
    } else {
        this->name.assign(name, pos + 1, std::string::npos);
        prefix.assign(name, 0, pos);
    }
}

std::string Attribute::getPrefixName() const
{
    return prefix.empty() ? name : (prefix + ":" + name);
}

const std::string& Attribute::getValue() const
{
    return value;
}

void Attribute::setValue(const std::string& value)
{
    this->value = value;
}


const Attribute* Attribute::getNextSibling() const
{
    return nextSibling;
}

Attribute* Attribute::getNextSibling()
{
    return nextSibling;
}


Attribute& Attribute::operator=(const Attribute& attr)
{
    prefix = attr.prefix;
    name = attr.name;
    value = attr.value;
    return *this;
}

bool Attribute::operator==(const Attribute& attr) const
{
    return name == attr.name && value == attr.value && prefix == attr.prefix;
}

bool Attribute::operator!=(const Attribute& attr) const
{
    return !(operator==(attr));
}

} // namespace xml
} // namespace ngrest

