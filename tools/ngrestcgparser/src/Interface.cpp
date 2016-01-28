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

#include <ngrest/utils/Exception.h>

#include "Interface.h"

namespace ngrest {
namespace codegen {

BaseType::BaseType(Type type_):
    type(type_)
{
}

BaseType& BaseType::operator=(const BaseType& other)
{
    NGREST_ASSERT_PARAM(type == other.type);
    name = other.name;
    ns = other.ns;
    ownerName = other.ownerName;
    description = other.description;
    details = other.details;
    isExtern = other.isExtern;
    isForward = other.isForward;
    options = other.options;
    return *this;
}

Enum::Enum():
    BaseType(Type::Enum)
{
}

Enum& Enum::operator=(const Enum& other)
{
    BaseType::operator=(static_cast<const BaseType&>(other));
    members = other.members;
    return *this;
}

Struct::Struct():
    BaseType(Type::Struct)
{
}

Struct& Struct::operator=(const Struct& other)
{
    BaseType::operator=(static_cast<const BaseType&>(other));
    parentName = other.parentName;
    parentNs = other.parentNs;
    fields = other.fields;
    structs = other.structs;
    enums = other.enums;
    return *this;
}

Typedef::Typedef():
    BaseType(Type::Typedef)
{
}

Typedef& Typedef::operator=(const Typedef& other)
{
    BaseType::operator=(static_cast<const BaseType&>(other));
    dataType = other.dataType;
    return *this;
}

Service::Service():
    BaseType(Type::Service)
{
}

Service& Service::operator=(const Service& other)
{
    BaseType::operator=(static_cast<const BaseType&>(other));
    modules = other.modules;
    operations = other.operations;
    return *this;
}

}
}
