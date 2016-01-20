##foreach $(interface.typedefs)
##include "nsopt.cpp"
struct $(typedef.name)Serializer
{
    inline static void serialize(::ngrest::MessageContext* context, \
##ifeq($(typedef.dataType.type),generic||enum)
$(typedef.nsName)\
##else
const $(typedef.nsName)&\
##endif
 value, ::ngrest::Node*& node)
    {
##context $(typedef.dataType)
##pushvars
##var var value
##var node node
##var name value
##include <common/serialization.cpp>
##popvars
##endcontext
    }

    inline static void deserialize(const ::ngrest::Node* node, $(typedef.name)& value)
    {
##context $(typedef.dataType)
##pushvars
##var var value
##var node node
##var name value
##include <common/deserialization.cpp>
##popvars
##endcontext
    }
};

##endfor
