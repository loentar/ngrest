// DESERIALIZE: $(.nsName) $(.type)
\
##switch $(.type)
\
##case generic||string
    ::ngrest::ObjectModelUtils::getValue($($node), $($var));
##case enum
    $($var) = $(.ns)$(.name.!replace/::/Serializer::/)Serializer::fromCString(::ngrest::ObjectModelUtils::getChildValue($($node), "$($name)"));
##case struct||typedef
    const ::ngrest::NamedNode* $($name)Node = ::ngrest::ObjectModelUtils::getNamedChild(static_cast<const ::ngrest::Object*>($($node)), "$($name)", ::ngrest::NodeType::Object);
    $(.ns)$(.name.!replace/::/Serializer::/)Serializer::deserialize($($name)Node->node, $($var));
##case template
\
    // count = $(.templateParams.$count) / $(.name)
##switch $(.name)
\
### /// list
##case vector||list
    NGREST_ASSERT_NULL($($node));
    NGREST_ASSERT($($node)->type == ::ngrest::NodeType::Array, "Array node type expected");
    const ::ngrest::Array* $($name)Arr = static_cast<const ::ngrest::Array*>($($node));
    for (const ::ngrest::LinkedNode* $($name)Child = $($name)Arr->firstChild; $($name)Child; $($name)Child = $($name)Child->nextSibling) {
##ifneq($(.templateParams.templateParam1.type),generic||enum)
        $($var).push_back($(.templateParams.templateParam1.nsName)());
        $(.templateParams.templateParam1.nsName)& $($name)Item = $($var).back();
##else
        $(.templateParams.templateParam1.nsName) $($name)Item;
##endif
\
##context $(.templateParams.templateParam1)
##pushvars
##var node $($name)Child->node
##var var $($name)Item
##var name $($name)Item
##indent +
##include "deserialization.cpp"
##indent -
##popvars
##endcontext
\
##ifeq($(.templateParams.templateParam1.type),generic||enum)
        $($var).push_back($($name)Item);
##endif
    }
\
### /// map
##case map||unordered_map
    NGREST_ASSERT($($node)->type == ::ngrest::NodeType::Object, "Object node type expected");
    const ::ngrest::Object* $($name)Obj = static_cast<const ::ngrest::Object*>($($node));
    for (const ::ngrest::NamedNode* $($name)Child = static_cast<const ::ngrest::NamedNode*>($($name)Obj->firstChild); $($name)Child; $($name)Child = $($name)Child->nextSibling) {
        NGREST_ASSERT_NULL($($name)Child->name);
##switch $(.templateParams.templateParam1.type)
##case generic
        $(.templateParams.templateParam1.nsName) $($name)Key;
        NGREST_ASSERT(::ngrest::fromCString($($name)Child->name, $($name)Key), "Cannot deserialize key of $($var)");
##case enum
        $(.templateParams.templateParam1.nsName) $($name)Key = $(param.dataType.templateParams.templateParam1.nsName)Serializer::fromCString($(param.name)Child->name);
##case string
##default
##error Cannot deserialize $(.templateParams.templateParam1.nsName) as map key
##endswitch

        $(.templateParams.templateParam2.nsName)& $($name)Value = $($var)\
##ifeq($(.templateParams.templateParam1.type),string)
[$($name)Child->name];
##else
[$($name)Key];
##endif
\
##context $(.templateParams.templateParam2)
##pushvars
##var node $($name)Child->node
##var var $($name)Value
##var name $($name)Value
##indent +
##include "deserialization.cpp"
##indent -
##popvars
##endcontext
    } // for(NamedNode...
\
### /// unsupported
##default
##error Serialization of template type $(.nsName) is not implemented
##endswitch
### /// end of template serialization
\
##default
##error unsupported type $(.type) :: $(.nsName)
##endswitch
\
// END DESERIALIZE: $(.nsName) $(.type)
