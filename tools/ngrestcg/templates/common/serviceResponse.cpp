##ifneq($($thisElementValue),void)
/// ######### serialize response ###########
        ::ngrest::Object* responseNode = context->pool.alloc< ::ngrest::Object>();
        ::ngrest::NamedNode* resultNode = context->pool.alloc< ::ngrest::NamedNode>("$(operation.options.*resultElement||"result")");
        responseNode->firstChild = resultNode;

##switch $(.type)
\
##case generic
##ifneq($(.name.!match/bool/),true)
        char resultBuff[NUM_TO_STR_BUFF_SIZE];
        NGREST_ASSERT(::ngrest::toCString(result, resultBuff, NUM_TO_STR_BUFF_SIZE), "Failed to serialize result for $(service.name)/$(operation.name)");
##endif
        resultNode->node = context->pool.alloc< ::ngrest::Value>(::ngrest::ValueType::\
##ifeq($(.name.!match/bool/),true)
Boolean\
##else
Number\
##endif
, \
##ifneq($(.name.!match/bool/),true)
resultBuff\
##else
result ? "true" : "false"\
##endif
);
##case string
        resultNode->node = context->pool.alloc< ::ngrest::Value>(::ngrest::ValueType::String, result.c_str());
##case enum
        resultNode->node = context->pool.alloc< ::ngrest::Value>(::ngrest::ValueType::String, $(.ns)$(.name.!replace/::/Serializer::/)Serializer::toCString(result));
##case struct||typedef
        resultNode->node = context->pool.alloc< ::ngrest::Object>();
        $(.ns)$(.name.!replace/::/Serializer::/)Serializer::serialize(context, result, resultNode->node);
##case template
\
// count = $(.templateParams.$count)
##switch $(.name)
\
### /// list
##case vector||list
        ::ngrest::Array* resultArray = context->pool.alloc< ::ngrest::Array>();
        resultNode->node = resultArray;
        ::ngrest::LinkedNode* lastResultArrayItem = nullptr;
        for (auto it = result.begin(), end = result.end(); it != end; ++it) {
            ::ngrest::LinkedNode* resultArrayItem = context->pool.alloc< ::ngrest::LinkedNode>();
            if (lastResultArrayItem == nullptr) {
                resultArray->firstChild = resultArrayItem;
            } else {
                lastResultArrayItem->nextSibling = resultArrayItem;
            }
            lastResultArrayItem = resultArrayItem;
\
##context $(.templateParams.templateParam1)
##pushvars
##var var (*it)
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
        ::ngrest::Object* resultObj = context->pool.alloc< ::ngrest::Object>();
        resultNode->node = resultObj;
        ::ngrest::NamedNode* lastResultObjItem = nullptr;
        for (auto it = result.begin(), end = result.end(); it != end; ++it) {
### // key
\
##var inlineValue
##switch $(.templateParams.templateParam1.type)
##case generic
##ifneq($(.templateParams.templateParam1.name.!match/bool/),true)
        char resultBuffItem[NUM_TO_STR_BUFF_SIZE];
        NGREST_ASSERT(::ngrest::toCString(it->first, resultBuffItem, NUM_TO_STR_BUFF_SIZE), "Failed to serialize result for $(service.name)/$(operation.name)");
##var inlineValue resultBuffItem
##else
##var inlineValue it->first ? "true" : "false"
##endif
##case string
##var inlineValue it->first.c_str()
##case enum
##var inlineValue $(.templateParams.templateParam1.ns)$(.templateParams.templateParam1.name.!replace/::/Serializer::/)Serializer::toCString(it->first)
##default
##error Cannot serialize $(.templateParams.templateParam1) as response of $(service.name)/$(operation.name)
##endswitch
\
            ::ngrest::NamedNode* resultObjItem = context->pool.alloc< ::ngrest::NamedNode>($($inlineValue));
            if (lastResultObjItem == nullptr) {
                resultObj->firstChild = resultObjItem;
            } else {
                lastResultObjItem->nextSibling = resultObjItem;
            }
            lastResultObjItem = resultObjItem;

\
##context $(.templateParams.templateParam2)
##pushvars
##var var it->second
##var node resultObjItem->node
##var name resultItem
##indent +2
##include <common/serialization.cpp>
##indent -2
##popvars
##endcontext
        }
\
### /// unsupported
##default
##error Serialization of template type $(.nsName) is not implemented
### /// end of template
##endswitch
\
##default
##error Serialization of type is not supported: $($thisElementValue): $(.type) :: $(service.name)/$(operation.name)
##endswitch

        context->response->node = responseNode;
        context->callback->success(context);
/// ######### serialize response end ###########

##endif
