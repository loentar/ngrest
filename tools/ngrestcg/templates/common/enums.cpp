##foreach $(interface.enums)
##include "nsopt.cpp"
struct $(enum.name)Serializer
{
    inline static const char* toCString($(enum.name) value)
    {
        switch (value) {
##foreach $(enum.members)
        case $(member.name):
            return "$(member.name)";
##endfor
        default:
            NGREST_THROW_ASSERT("Can't serialize enum $(enum.name) from value: [" + toString(static_cast<int>(value)) + "]");
        }
    }

    inline static $(enum.name) fromCString(const char* str)
    {
##foreach $(enum.members)
        if (!strcmp(str, "$(member.name)"))
            return $(member.name);
##endfor
        NGREST_THROW_ASSERT("Can't deserialize enum $(enum.name) from value: [" + std::string(str) + "]");
    }
};

##endfor
