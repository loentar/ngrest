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

#include <sstream>
#include "Attribute.h"
#include "Exception.h"
#include "Declaration.h"

namespace ngrest {
namespace xml {

Declaration::Declaration():
    version("1.0"),
    encoding("UTF-8"),
    standalone(Standalone::Undefined)
{
}

Declaration::~Declaration()
{
}

const std::string& Declaration::getVersion() const
{
    return version;
}

void Declaration::setVersion(const std::string& version)
{
    this->version = version;
}

const std::string& Declaration::getEncoding() const
{
    return encoding;
}

void Declaration::setEncoding(const std::string& encoding)
{
    this->encoding = encoding;
}

Declaration::Standalone Declaration::getStandalone() const
{
    return standalone;
}

void Declaration::setStandalone(Declaration::Standalone standalone)
{
    this->standalone = standalone;
}

} // namespace xml
} // namespace ngrest
