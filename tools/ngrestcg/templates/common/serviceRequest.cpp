##var callbackType
##var callbackParam
##ifneq($(operation.params.$count),0)
/// ######### deserialize request ###########
##ifneq($(operation.params.$count)-$(operation.params.param.dataType.name),1-MessageContext||1-Callback)
        NGREST_ASSERT(context->request->node, "Request expected for $(service.serviceNsName)/$(operation.name)");
        NGREST_ASSERT_PARAM(context->request->node->type == ::ngrest::NodeType::Object);

        const ::ngrest::Object* request = static_cast<const ::ngrest::Object*>(context->request->node);
##endif

######### parameters ###########
\
######### deserialize request parameters ###########
##foreach $(operation.params)
\
##ifneq($(param.dataType.name),Callback||MessageContext)
        $(param.dataType.nsName) $(param.name);
##endif
##endfor

##foreach $(operation.params)
\
##switch $(param.dataType.type)
\
##case generic||string
##ifneq($(param.dataType.name),MessageContext)
        ::ngrest::ObjectModelUtils::getChildValue(request, "$(param.name)", $(param.name));
##endif
##case enum
        $(param.name) = $(param.dataType.ns)$(param.dataType.name.!replace/::/Serializer::/)Serializer::fromCString(::ngrest::ObjectModelUtils::getChildValue(request, "$(param.name)"));
##case struct
        const ::ngrest::NamedNode* $(param.name)Obj = ::ngrest::ObjectModelUtils::getNamedChild(request, "$(param.name)", ::ngrest::NodeType::Object);
        $(param.dataType.ns)$(param.dataType.name.!replace/::/Serializer::/)Serializer::deserialize($(param.name)Obj->node, $(param.name));
### we dont know typedef type here, so take any
##case typedef
        const ::ngrest::NamedNode* $(param.name)Obj = ::ngrest::ObjectModelUtils::getNamedChild(request, "$(param.name)");
        $(param.dataType.ns)$(param.dataType.name.!replace/::/Serializer::/)Serializer::deserialize($(param.name)Obj->node, $(param.name));
##case template
\
        // count = $(param.dataType.templateParams.$count)
##switch $(param.dataType.name)
\
### /// Callback for asynchronous operations
##case Callback
##ifneq($($callbackType),)
##error More than one callback defined for $(service.name)/$(operation.name)
##endif
##ifneq($(operation.return),void)
##error Return type of asynchronous operation must be void: $(service.name)/$(operation.name)
##endif
##var callbackType $(param.dataType.templateParams.templateParam1)
##var callbackParam $(param.name)
### /// list
##case vector||list
        const ::ngrest::NamedNode* $(param.name)Node = ::ngrest::ObjectModelUtils::getNamedChild(request, "$(param.name)", ::ngrest::NodeType::Array);
        for (const ::ngrest::LinkedNode* $(param.name)Child = static_cast<const ::ngrest::Array*>($(param.name)Node->node)->firstChild; $(param.name)Child; $(param.name)Child = $(param.name)Child->nextSibling) {
##ifneq($(param.dataType.templateParams.templateParam1.type),generic||enum)
            $(param.name).push_back($(param.dataType.templateParams.templateParam1.nsName)());
            $(param.dataType.templateParams.templateParam1.nsName)& $(param.name)Item = $(param.name).back();
##else
            $(param.dataType.templateParams.templateParam1.nsName) $(param.name)Item;
##endif
\
##context $(param.dataType.templateParams.templateParam1)
##pushvars
##var var $(param.name)Item
##var node $(param.name)Child->node
##var name $(param.name)Item
##indent +2
##include <common/deserialization.cpp>
##indent -2
##popvars
##endcontext
\
##ifeq($(param.dataType.templateParams.templateParam1.type),generic||enum)
            $(param.name).push_back($(param.name)Item);
##endif
        }
\
### /// map
##case map||unordered_map
        const ::ngrest::NamedNode* $(param.name)Node = ::ngrest::ObjectModelUtils::getNamedChild(request, "$(param.name)", ::ngrest::NodeType::Object);
        for (const ::ngrest::NamedNode* $(param.name)Child = static_cast<const ::ngrest::Object*>($(param.name)Node->node)->firstChild; $(param.name)Child; $(param.name)Child = $(param.name)Child->nextSibling) {
            NGREST_ASSERT_NULL($(param.name)Child->name);
##switch $(param.dataType.templateParams.templateParam1.type)
##case generic
            $(param.dataType.templateParams.templateParam1.nsName) $(param.name)Key;
            NGREST_ASSERT(::ngrest::fromCString($(param.name)Child->name, $(param.name)Key), "Cannot deserialize key of $(param.name)");
##case enum
            $(param.dataType.templateParams.templateParam1.nsName) $(param.name)Key = $(param.dataType.templateParams.templateParam1.nsName)Serializer::fromCString($(param.name)Child->name);
##case string
        // inline $(param.name)Child->name
##default
##error Cannot deserialize $(param.dataType.templateParams.templateParam1.nsName) as map key
##endswitch

            $(param.dataType.templateParams.templateParam2.nsName)& $(param.name)Value = $(param.name)\
##ifeq($(param.dataType.templateParams.templateParam1.type),string)
[$(param.name)Child->name];
##else
[$(param.name)Key];
##endif
\
##context $(param.dataType.templateParams.templateParam2)
##pushvars
##var var $(param.name)Value
##var node $(param.name)Child->node
##var name $(param.name)Value
##indent +2
##include <common/deserialization.cpp>
##indent -2
##popvars
##endcontext
\
        }
\
##case Nullable
##context $(param.dataType.templateParams.templateParam1)
##pushvars
##var name $(param.name)
##var node namedNode$($name)->node
        const ::ngrest::NamedNode* namedNode$($name) = request->findChildByName("$(param.name)");
        if (namedNode$($name) != nullptr && namedNode$($name)->node != nullptr) {
##ifeq($(.type),generic||string||enum)
##var var $(param.name).get()
##else
##var var $(param.name)Nullable
            $(.nsName)& $($var) = $(param.name).get();
##endif
##indent +2
##include <common/deserialization.cpp>
##indent -2
        }
##popvars
##endcontext
### /// unsupported
##default
##error Deserialization of template type $(param.dataType.nsName) is not supported
### /// end of template
##endswitch
\
##default
##error Deserialization of type is not supported: $(param): $(param.dataType.type) :: $(param.dataType) :: $(operation.name)
##endswitch
\
##endfor
/// ######### deserialize request end ###########

##endif // parameters count
