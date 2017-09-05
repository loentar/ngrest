##foreach $(.structs)
##ifeq($(struct.isExtern),false)
##include "nsopt.cpp"
struct $(struct.name)Serializer
{
##ifneq($(struct.structs.$count),0)
##indent +
##include "structs.h"
##indent -

##endif
##ifneq($(struct.enums.$count),0)
##indent +
##include "enums.h"
##indent -

##endif
    static void serialize(::ngrest::MessageContext* context, const $(struct.ownerName)& value, ::ngrest::Node* node);
    static void deserialize(const ::ngrest::Node* node, $(struct.ownerName)& value);
};

##endif
##endfor
