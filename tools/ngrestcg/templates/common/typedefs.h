##foreach $(interface.typedefs)
##ifeq($(typedef.isExtern),false)
##include "nsopt.cpp"
struct $(typedef.name)Serializer
{
    static void serialize(::ngrest::MessageContext* context, \
##ifeq($(typedef.dataType.type),generic||enum)
$(typedef.name)\
##else
const $(typedef.name)&\
##endif
 value, ::ngrest::Node*& node);
    static void deserialize(const ::ngrest::Node* node, $(typedef.name)& value);
};

##endif
##endfor
