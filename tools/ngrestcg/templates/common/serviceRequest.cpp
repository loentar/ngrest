##var callbackType
##var callbackParam
##ifneq($(operation.params.$count),0)
/// ######### deserialize request ###########
        NGREST_ASSERT(context->request->node, "Request expected for $(service.serviceNsName)/$(operation.name)");
        NGREST_ASSERT_PARAM(context->request->node->type == NodeType::Object);

        const ::ngrest::Object* request = static_cast<const ::ngrest::Object*>(context->request->node);

######### parameters ###########
\
######### deserialize request parameters ###########
##foreach $(operation.params)
\
##ifneq($(param.dataType.name),Callback)
        $(param.dataType.nsName) $(param.name);
##endif
##endfor

##foreach $(operation.params)
\
##switch $(param.dataType.type)
\
##case generic||string
        ::ngrest::ObjectModelUtils::getChildValue(request, "$(param.name)", $(param.name));
##case enum
        $(param.name) = $(param.dataType.nsName)Serializer::fromCString(::ngrest::ObjectModelUtils::getChildValue(request, "$(param.name)"));
##case struct||typedef
        const ::ngrest::NamedNode* $(param.name)Obj = ::ngrest::ObjectModelUtils::getNamedChild(request, "$(param.name)", ::ngrest::NodeType::Object);
        $(param.dataType.nsName)Serializer::deserialize($(param.name)Obj->node, $(param.name));
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
##include <common/deserialization.cpp>
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
##include <common/deserialization.cpp>
##popvars
##endcontext
\
        }
\
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
