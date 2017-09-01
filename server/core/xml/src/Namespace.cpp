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

