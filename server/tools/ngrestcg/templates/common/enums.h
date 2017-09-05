##foreach $(.enums)
##ifeq($(enum.isExtern),false)
##include "nsopt.cpp"
struct $(enum.name)Serializer
{
    static const char* toCString($(enum.ownerName) value);
    static $(enum.ownerName) fromCString(const char* str);
};

##endif
##endfor
