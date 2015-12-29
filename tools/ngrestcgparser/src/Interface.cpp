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
