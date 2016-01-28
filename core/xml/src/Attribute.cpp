/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

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

