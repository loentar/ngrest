// DESERIALIZE: $(.nsName) $(.type)
##indent +
\
##switch $(.type)
\
##case generic||string
    ::ngrest::ObjectModelUtils::getValue($($node), $($var));
##case enum
    $($var) = $(.nsName)Serializer::fromCString(::ngrest::ObjectModelUtils::getChildValue($($node), "$($var)"));
##case struct||typedef
    const ::ngrest::NamedNode* $($var)Node = ::ngrest::ObjectModelUtils::getNamedChild(static_cast<const ::ngrest::Object*>($($node)), "$($var)", ::ngrest::NodeType::Object);
    $(.nsName)Serializer::deserialize($($var)Node->node, $($var));
##case template
\
    // count = $(.templateParams.$count) / $(.name)
##switch $(.name)
\
### /// list
##case vector||list
    NGREST_ASSERT_NULL($($node));
    NGREST_ASSERT($($node)->type == ::ngrest::NodeType::Array, "Array node type expected");
    const ::ngrest::Array* $($var)Arr = static_cast<const ::ngrest::Array*>($($node));
    for (const ::ngrest::LinkedNode* $($var)Child = $($var)Arr->firstChild; $($var)Child; $($var)Child = $($var)Child->nextSibling) {
##ifneq($(.templateParams.templateParam1.type),generic||enum)
        $($var).push_back($(.templateParams.templateParam1.nsName)());
        $(.templateParams.templateParam1.nsName)& $($var)Item = $($var).back();
##else
        $(.templateParams.templateParam1.nsName) $($var)Item;
##endif
\
##context $(.templateParams.templateParam1)
##pushvars
##var node $($var)Child->node
##var var $($var)Item
##include "deserialization.cpp"
##popvars
##endcontext
\
##ifeq($(.templateParams.templateParam1.type),generic||enum)
        $($var).push_back($($var)Item);
##endif
    }
\
### /// map
##case map||unordered_map
    NGREST_ASSERT($($node)->type == ::ngrest::NodeType::Object, "Object node type expected");
    const ::ngrest::Object* $($var)Obj = static_cast<const ::ngrest::Object*>($($node));
    for (const ::ngrest::NamedNode* $($var)Child = static_cast<const ::ngrest::NamedNode*>($($var)Obj->firstChild); $($var)Child; $($var)Child = $($var)Child->nextSibling) {
        NGREST_ASSERT_NULL($($var)Child->name);
##switch $(.templateParams.templateParam1.type)
##case generic
        $(.templateParams.templateParam1.nsName) $($var)Key;
        NGREST_ASSERT(::ngrest::fromCString($($var)Child->name, $($var)Key), "Cannot deserialize key of $($var)");
##case enum
        $(.templateParams.templateParam1.nsName) $($var)Key = $(param.dataType.templateParams.templateParam1.nsName)Serializer::fromCString($(param.name)Child->name);
##case string
##default
##error Cannot deserialize $(.templateParams.templateParam1.nsName) as map key
##endswitch

        $(.templateParams.templateParam2.nsName)& $($var)Value = $($var)\
##ifeq($(.templateParams.templateParam1.type),string)
[$($var)Child->name];
##else
[$($var)Key];
##endif
\
##context $(.templateParams.templateParam2)
##pushvars
##var node $($var)Child->node
##var var $($var)Value
##include <common/deserialization.cpp>
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
##indent -
// END DESERIALIZE: $(.nsName) $(.type)
