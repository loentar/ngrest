/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

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

Service* TestDeploymentWrapper::getServiceImpl()
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

        Object* responseNode = context->pool->alloc<Object>();

        NamedNode* resultNode = context->pool->alloc<NamedNode>("result");
        resultNode->node = context->pool->alloc<Value>(ValueType::String, result.c_str());

        responseNode->firstChild = resultNode;

        context->response->node = responseNode;

        context->callback->success();
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
                Object* responseNode = context->pool->alloc<Object>();

                NamedNode* resultNode = context->pool->alloc<NamedNode>("result");
                resultNode->node = context->pool->alloc<Value>(ValueType::String, result.c_str());

                responseNode->firstChild = resultNode;

                context->response->node = responseNode;

                context->callback->success();
                // no need to "delete this" - it's in mempool
            }

            virtual void error(const Exception& error)
            {
                context->callback->error(error);
                // no need to "delete this" - it's in mempool
            }

            MessageContext* context;
        };

        service->echoASync(value, *context->pool->alloc<Callback_echoASync>(context));

    } else {
        NGREST_THROW_ASSERT("No operation " + operation->name + " found");
    }
}

const ServiceDescription* TestDeploymentWrapper::getDescription() const
{
    static ServiceDescription description = {
        "ngrest.TestDeployment",
        "td",
        "",
        "",
        {
            {
                "echoSync",
                "sync?value={value}",
                static_cast<int>(HttpMethod::GET),
                "GET",
                false,
                "",
                "",
                {
                    {"value", ParameterDescription::Type::String, false}
                },
                ParameterDescription::Type::String,
                false
            },
            {
                "echoASync",
                "async/{value}",
                static_cast<int>(HttpMethod::GET),
                "GET",
                true,
                "",
                "",
                {
                    {"value", ParameterDescription::Type::String, false}
                },
                ParameterDescription::Type::String,
                false
            }
        }
    };

    return &description;
}

} // namespace ngrest

