// DESERIALIZE: $(.nsName) $(.type)
\
### fix node for container types
##ifeq($(.type),template)
##switch $(.name)
##case list||vector
    NGREST_ASSERT_NULL($($node));
    NGREST_ASSERT($($node)->type == ::ngrest::NodeType::Array, "Array node expected");
##var node static_cast<const ::ngrest::Array*>($($node))
##case map||unordered_map
    NGREST_ASSERT_NULL($($node));
    NGREST_ASSERT($($node)->type == ::ngrest::NodeType::Object, "Object node expected");
##var node static_cast<const ::ngrest::Object*>($($node))
##case Nullable
    if ($($node) != nullptr) {
##context $(.templateParams.templateParam1)
##pushvars
##ifeq($(.type),template)
    $(.nsName)& $($var.!replace/./_/)$(.name) = $($var).get();
##var var $($var.!replace/./_/)$(.name)
##else
##var var $($var).get()
##endif
##indent +
##include "deserialization.cpp"
##indent -
##popvars
##endcontext
    }
##endswitch
##endif
\
##switch $(.type)
\
##case generic||string
    ::ngrest::ObjectModelUtils::getValue($($node), $($var));
##case enum
    $($var) = $(.ns)$(.name.!replace/::/Serializer::/)Serializer::fromCString(::ngrest::ObjectModelUtils::getValue($($node)));
##case struct||typedef
    $(.ns)$(.name.!replace/::/Serializer::/)Serializer::deserialize($($node), $($var));
##case template
\
    // count = $(.templateParams.$count) / $(.name) / $($var) / $($name) / $($node)
##switch $(.name)
\
### /// list
##case vector||list
    for (const ::ngrest::LinkedNode* $($name)Child = $($node)->firstChild; $($name)Child; $($name)Child = $($name)Child->nextSibling) {
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
    for (const ::ngrest::NamedNode* $($name)Child = $($node)->firstChild; $($name)Child; $($name)Child = $($name)Child->nextSibling) {
        NGREST_ASSERT_NULL($($name)Child->name);
        // deserialize key
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
        // deserialize value
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
##case Nullable
### already processed at top of thils file
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
