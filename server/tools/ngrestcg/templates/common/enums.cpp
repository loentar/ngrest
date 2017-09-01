##foreach $(.enums)
##ifeq($(enum.isExtern),false)
##include "nsopt.cpp"

// enum $(enum.name)

const char* $(enum.ownerName.!replace/::/Serializer::/)Serializer::toCString($(enum.ownerName) value)
{
    switch (value) {
##foreach $(enum.members)
    case $(enum.ownerName)::$(member.name):
        return "$(member.name)";
##endfor
    default:
        NGREST_THROW_ASSERT("Can't serialize enum $(enum.name) from value: [" + ::ngrest::toString(static_cast<int>(value)) + "]");
    }
}

$(enum.ownerName) $(enum.ownerName.!replace/::/Serializer::/)Serializer::fromCString(const char* str)
{
##foreach $(enum.members)
    if (!strcmp(str, "$(member.name)"))
        return $(enum.ownerName)::$(member.name);
##endfor
    NGREST_THROW_ASSERT("Can't deserialize enum $(enum.name) from value: [" + std::string(str) + "]");
}

##endif
##endfor
