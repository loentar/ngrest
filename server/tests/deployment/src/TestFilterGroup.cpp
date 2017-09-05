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

#include <algorithm>

#include <core/utils/stringutils.h>
#include <ngrest/common/Message.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/common/ObjectModelUtils.h>
#include <ngrest/engine/Phase.h>
#include <ngrest/engine/Filter.h>
#include <core/utils/Log.h>

#include "TestFilterGroup.h"


namespace ngrest {

class TestFilterHeader: public Filter
{
    const std::string& getName() const override
    {
        static const std::string name = "TestFilter";
        return name;
    }

    const std::list<std::string>& getDependencies() const override
    {
        static const std::list<std::string> dependencies = {};
        return dependencies;
    }

    void filter(Phase phase, MessageContext* context) override
    {
        // you can throw HttpException to stop processing request

        switch (phase) {
        case Phase::Header: {
            // you can read request headers here

            // log all headers in request
            for (Header* header = context->request->headers; header; header = header->next) {
                LogDebug() << "header: " << header->name << " : " << header->value;
            }
            break;
        }

        case Phase::PreDispatch: {
            // to work with raw request buffer

            if (!context->request->body) { // GET, DELETE
                // replace 1 to _ONE_ in request url
                NGREST_ASSERT_NULL(context->request->path);
                std::string newPath = context->request->path;
                stringReplace(newPath, "1", "_ONE_", true);
                context->request->path = context->pool->putCString(newPath.c_str(), newPath.size(), true);
            } else {  // POST, PUT
                // body is stored in external memory pool
                // replace any 1 to ! in raw request
                char* request = context->request->body;
                uint64_t size = context->request->bodySize;
                char* requestEnd = request + size;
                char* pos = request;
                while ((pos = std::find(pos, requestEnd, '1')) != requestEnd) {
                    *pos = '!';
                }

                // when changing request memory pool need to call flattern() and update
                // context->request->body and context->request->bodySize
            }

            break;
        }

        case Phase::PreInvoke: {
            // to work with parsed OM request
            if (!context->request->node) {
                LogWarning() << "skipping PreInvoke phase: don't have OM";
                return;
            }

            if (context->request->node->type != NodeType::Object) {
                LogWarning() << "Unexpected node type";
                return;
            }

            Object* obj = static_cast<Object*>(context->request->node);
            NamedNode* valueNode = obj->findChildByName("value");
            if (!valueNode) {
                LogWarning() << "can't find node value";
                return;
            }

            if (!valueNode->node || valueNode->node->type != NodeType::Value)  {
                LogWarning() << "Unexpected node type";
                return;
            }

            Value* val = static_cast<Value*>(valueNode->node);
            std::string newVal = val->value;

            stringReplace(newVal, "2", "_two_", true);

            val->value = context->pool->putCString(newVal.c_str(), newVal.size(), true);

            break;
        }

        case Phase::PostDispatch: {
            // to work with OM response

            // prepend header to headers
            context->response->headers = context->pool->alloc<Header>("X-TestHeader", "Some Value",
                                                                      context->response->headers);

            if (!context->response->node) {
                LogWarning() << "skipping PostDispatch node phase";
                return;
            }

            // ...
            break;
        }

        case Phase::PreSend:
            if (!context->response->poolBody) {
                LogWarning() << "skipping PreSend phase";
                return;
            }

            // ...
            break;

        default:;
        }
    }
};


TestFilterGroup::TestFilterGroup():
  filter(new TestFilterHeader()),
  filters({
      {Phase::Header, {filter}},
      {Phase::PreDispatch, {filter}},
      {Phase::PreInvoke, {filter}},
      {Phase::PostDispatch, {filter}},
      {Phase::PreSend, {filter}},
  })
{
}

TestFilterGroup::~TestFilterGroup()
{
    filters.clear();
    delete filter;
}

const std::string& TestFilterGroup::getName() const
{
    static const std::string name = "TestFilterGroup";
    return name;
}

const FiltersMap& TestFilterGroup::getFilters() const
{
    return filters;
}

}
