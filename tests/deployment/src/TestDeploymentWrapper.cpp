#include <ngrest/utils/Exception.h>
#include <ngrest/common/HttpMethod.h>
#include <ngrest/common/Message.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/engine/ServiceDescription.h>

#include "TestDeployment.h"
#include "TestDeploymentWrapper.h"

namespace ngrest {

TestDeploymentWrapper::TestDeploymentWrapper():
    service(new TestDeployment())
{

}

TestDeploymentWrapper::~TestDeploymentWrapper()
{
    delete service;
}

Service* TestDeploymentWrapper::serviceImpl()
{
    return service;
}

void TestDeploymentWrapper::invoke(const OperationDescription* operation, MessageContext* context)
{
    if (operation->name == "echoSync") {
        NGREST_ASSERT(context->request->node, "Request expected for Service:operation");
        NGREST_ASSERT_PARAM(context->request->node->type == NodeType::Object);

        const Object* requestNode = static_cast<const Object*>(context->request->node);

        NamedNode* valueNode = requestNode->findChildByName("value");
        NGREST_ASSERT_NULL(valueNode);
        NGREST_ASSERT_PARAM(valueNode->node->type == NodeType::Value);
        const char* value = static_cast<const Value*>(valueNode->node)->value;
        NGREST_ASSERT_NULL(value);


        const std::string& result = service->echoSync(value);

        Object* responseNode = context->pool.alloc<Object>();

        NamedNode* resultNode = context->pool.alloc<NamedNode>("result");
        resultNode->node = context->pool.alloc<Value>(ValueType::String, result.c_str());

        responseNode->firstChild = resultNode;

        context->response->node = responseNode;

        context->callback->success(context);
    } else if (operation->name == "echoASync") {
        NGREST_ASSERT(context->request->node, "Request expected for Service:operation");
        NGREST_ASSERT_PARAM(context->request->node->type == NodeType::Object);

        const Object* requestNode = static_cast<const Object*>(context->request->node);

        NamedNode* valueNode = requestNode->findChildByName("value");
        NGREST_ASSERT_NULL(valueNode);
        NGREST_ASSERT_PARAM(valueNode->node->type == NodeType::Value);
        const char* value = static_cast<const Value*>(valueNode->node)->value;
        NGREST_ASSERT_NULL(value);

        class Callback_echoASync: public Callback<const std::string&>
        {
        public:
            Callback_echoASync(MessageContext* context_):
                context(context_)
            {
            }

            void success(const std::string& result) override
            {
                Object* responseNode = context->pool.alloc<Object>();

                NamedNode* resultNode = context->pool.alloc<NamedNode>("result");
                resultNode->node = context->pool.alloc<Value>(ValueType::String, result.c_str());

                responseNode->firstChild = resultNode;

                context->response->node = responseNode;

                context->callback->success(context);
                // no need to "delete this" - it's in mempool
            }

            virtual void error(const Exception& error)
            {
                context->callback->error(error);
                // no need to "delete this" - it's in mempool
            }

            MessageContext* context;
        };

        service->echoASync(value, context->pool.alloc<Callback_echoASync>(context));

    } else {
        NGREST_THROW_ASSERT("No operation " + operation->name + " found");
    }
}

const ServiceDescription* TestDeploymentWrapper::description()
{
    static ServiceDescription description = {
        true,
        "ngrest.TestDeployment",
        "td",
        {
            {
                "echoSync",
                "sync?value={value}",
                static_cast<int>(HttpMethod::GET),
                false
            },
            {
                "echoASync",
                "async/{value}",
                static_cast<int>(HttpMethod::GET),
                true
            }
        }
    };

    return &description;
}

} // namespace ngrest

