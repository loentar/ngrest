##foreach $(interface.typedefs)
##ifeq($(typedef.isExtern),false)
##include "nsopt.cpp"

// typedef $(typedef.name)

void $(typedef.name)Serializer::serialize(::ngrest::MessageContext* context, \
##ifeq($(typedef.dataType.type),generic||enum)
$(typedef.name)\
##else
const $(typedef.name)&\
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

void $(typedef.name)Serializer::deserialize(const ::ngrest::Node* node, $(typedef.name)& value)
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

##endif
##endfor
