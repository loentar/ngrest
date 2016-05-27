##foreach $(.structs)
##ifeq($(struct.isExtern),false)
##include "nsopt.cpp"

// struct $(struct.ownerName)

void $(.ns)$(.ownerName.!replace/::/Serializer::/)Serializer::serialize(::ngrest::MessageContext* context, const $(struct.ownerName)& value, ::ngrest::Node* node)
{
##ifneq($(struct.parentNsName),)
    // serialize parent struct
    $(struct.parentNs)$(struct.parentName.!replace/::/Serializer::/)Serializer::serialize(context, value, node);

##endif
##var lastNodeName
##foreach $(struct.fields)
    ::ngrest::NamedNode* $(field.name)Node = context->pool->alloc< ::ngrest::NamedNode>("$(field.name)");
##ifeq($($lastNodeName),)
    static_cast< ::ngrest::Object*>(node)->firstChild = $(field.name)Node;
##else
    $($lastNodeName)->nextSibling = $(field.name)Node;
##endif
##var lastNodeName $(field.name)Node
##pushvars
##var var value.$(field.name)
##var node $(field.name)Node->node
##var name $(field.name)
##context $(field.dataType)
##include <common/serialization.cpp>
##endcontext
##popvars
##endfor
}

void $(.ns)$(.ownerName.!replace/::/Serializer::/)Serializer::deserialize(const ::ngrest::Node* node, $(struct.ownerName)& value)
{
### // no need to assert node is Object, it's already checked
    const ::ngrest::Object* object = static_cast<const ::ngrest::Object*>(node);
##ifneq($(struct.parentNsName),)
    // deserialize parent struct
    $(struct.parentNs)$(struct.parentName.!replace/::/Serializer::/)Serializer::deserialize(node, value);

##endif
##foreach $(struct.fields)
##context $(field.dataType)
##pushvars
\
##var node object
##var var value.$(field.name)
##var name $(field.name)
##switch $(.type)
##case generic||string
    ::ngrest::ObjectModelUtils::getChildValue(object, "$(field.name)", $($var));
##case enum
    $($var) = $(.ns)$(.name.!replace/::/Serializer::/)Serializer::fromCString(::ngrest::ObjectModelUtils::getChildValue(object, "$(field.name)"));
##case struct||typedef
##var node $($name)Object
    const ::ngrest::Object* $($node) = static_cast<const ::ngrest::Object*>(::ngrest::ObjectModelUtils::getNamedChild(object, "$(field.name)", ::ngrest::NodeType::Object)->node);
##case template
##switch $(.name)
##case list||vector
##var node $($name)Array
    const ::ngrest::Array* $($node) = static_cast<const ::ngrest::Array*>(::ngrest::ObjectModelUtils::getNamedChild(object, "$(field.name)", ::ngrest::NodeType::Array)->node);
##case map||unordered_map
##var node $($name)Object
    const ::ngrest::Object* $($node) = static_cast<const ::ngrest::Object*>(::ngrest::ObjectModelUtils::getNamedChild(object, "$(field.name)", ::ngrest::NodeType::Object)->node);
### /// unsupported
##default
##error Cannot deserialize template $(.name)
##endswitch
##default
##error Cannot deserialize $(.name)
##endswitch
##ifneq($(.type),generic||string||enum) // already processed
##include <common/deserialization.cpp>
##endif
##popvars
##endcontext
##endfor
}

##ifneq($(struct.structs.$count),0)
##include "structs.cpp"
##endif

##ifneq($(struct.enums.$count),0)
##include "enums.cpp"
##endif

##endif
##endfor
