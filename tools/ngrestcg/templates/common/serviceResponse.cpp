##ifneq($($thisElementValue),void)
/// ######### serialize response ###########

##ifneq($(operation.options.*inlineResult||interface.options.*defaultInlineResult),1||true)
        ::ngrest::Object* responseNode = context->pool->alloc< ::ngrest::Object>();
        ::ngrest::NamedNode* resultNode = context->pool->alloc< ::ngrest::NamedNode>("$(operation.options.*resultElement||"result")");
        responseNode->firstChild = resultNode;
##var resultNodeNode resultNode->node
##else
##var resultNodeNode context->response->node
##endif

##switch $(.type)
\
##case generic
##ifneq($(.name.!match/bool/),true)
        char resultBuff[NGREST_NUM_TO_STR_BUFF_SIZE];
        NGREST_ASSERT(::ngrest::toCString(result, resultBuff, NGREST_NUM_TO_STR_BUFF_SIZE), "Failed to serialize result for $(service.name)/$(operation.name)");
        const char* resultCStr = context->pool->putCString(resultBuff, true);
##endif
        $($resultNodeNode) = context->pool->alloc< ::ngrest::Value>(::ngrest::ValueType::\
##ifeq($(.name.!match/bool/),true)
Boolean\
##else
Number\
##endif
, \
##ifneq($(.name.!match/bool/),true)
resultCStr\
##else
result ? "true" : "false"\
##endif
);
##case string
        $($resultNodeNode) = context->pool->alloc< ::ngrest::Value>(::ngrest::ValueType::String, result.c_str());
##case enum
        $($resultNodeNode) = context->pool->alloc< ::ngrest::Value>(::ngrest::ValueType::String, $(.ns)$(.name.!replace/::/Serializer::/)Serializer::toCString(result));
##case struct||typedef
        $($resultNodeNode) = context->pool->alloc< ::ngrest::Object>();
        $(.ns)$(.name.!replace/::/Serializer::/)Serializer::serialize(context, result, $($resultNodeNode));
##case template
\
// count = $(.templateParams.$count)
##switch $(.name)
\
### /// list
##case vector||list
        ::ngrest::Array* resultArray = context->pool->alloc< ::ngrest::Array>();
        $($resultNodeNode) = resultArray;
        ::ngrest::LinkedNode* lastResultArrayItem = nullptr;
        for (const auto& it : result) {
            ::ngrest::LinkedNode* resultArrayItem = context->pool->alloc< ::ngrest::LinkedNode>();
            if (lastResultArrayItem == nullptr) {
                resultArray->firstChild = resultArrayItem;
            } else {
                lastResultArrayItem->nextSibling = resultArrayItem;
            }
            lastResultArrayItem = resultArrayItem;
\
##context $(.templateParams.templateParam1)
##pushvars
##var var it
##var node resultArrayItem->node
##var name resultItem
##indent +2
##include <common/serialization.cpp>
##indent -2
##popvars
##endcontext
        }
\
### /// map
##case map||unordered_map
        ::ngrest::Object* resultObj = context->pool->alloc< ::ngrest::Object>();
        $($resultNodeNode) = resultObj;
        ::ngrest::NamedNode* lastResultObjItem = nullptr;
        for (const auto& it : result) {
### // key
\
##var inlineValue
##switch $(.templateParams.templateParam1.type)
##case generic
##ifneq($(.templateParams.templateParam1.name.!match/bool/),true)
        char resultBuffItem[NGREST_NUM_TO_STR_BUFF_SIZE];
        NGREST_ASSERT(::ngrest::toCString(it.first, resultBuffItem, NGREST_NUM_TO_STR_BUFF_SIZE), "Failed to serialize result for $(service.name)/$(operation.name)");
        const char* $($name)CStr = context->pool->putCString(resultBuffItem, true);
##var inlineValue $($name)CStr
##else
##var inlineValue it.first ? "true" : "false"
##endif
##case string
##var inlineValue it.first.c_str()
##case enum
##var inlineValue $(.templateParams.templateParam1.ns)$(.templateParams.templateParam1.name.!replace/::/Serializer::/)Serializer::toCString(it.first)
##default
##error Cannot serialize $(.templateParams.templateParam1) as response of $(service.name)/$(operation.name)
##endswitch
\
            ::ngrest::NamedNode* resultObjItem = context->pool->alloc< ::ngrest::NamedNode>($($inlineValue));
            if (lastResultObjItem == nullptr) {
                resultObj->firstChild = resultObjItem;
            } else {
                lastResultObjItem->nextSibling = resultObjItem;
            }
            lastResultObjItem = resultObjItem;

\
##context $(.templateParams.templateParam2)
##pushvars
##var var it.second
##var node resultObjItem->node
##var name resultItem
##indent +2
##include <common/serialization.cpp>
##indent -2
##popvars
##endcontext
        }
\
##case Nullable
        if (result.isValid()) {
##context $(.templateParams.templateParam1)
##pushvars
##var var (*result)
##var name result
##var node $($resultNodeNode)
##indent +2
##include <common/serialization.cpp>
##indent -2
##popvars
##endcontext
        }
### /// unsupported
##default
##error Serialization of template type $(.nsName) is not implemented
### /// end of template
##endswitch
\
##default
##error Serialization of type is not supported: $($thisElementValue): $(.type) :: $(service.name)/$(operation.name)
##endswitch
##ifneq($(operation.options.*inlineResult||interface.options.*defaultInlineResult),1||true)

        context->response->node = responseNode;
##endif
/// ######### serialize response end ###########

##endif
