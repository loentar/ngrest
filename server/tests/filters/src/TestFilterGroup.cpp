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
#include <core/utils/PluginExport.h>
#include <ngrest/common/Message.h>
#include <ngrest/common/HttpException.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/common/ObjectModelUtils.h>
#include <ngrest/engine/Phase.h>
#include <ngrest/engine/Filter.h>
#include <ngrest/engine/Transport.h>
#include <core/utils/Log.h>

#include "TestFilterGroup.h"

NGREST_DECLARE_PLUGIN(::ngrest::TestFilterGroup)

namespace ngrest {

class TestFilter: public Filter
{
public:
    TestFilter(const std::string& name_, const std::list<std::string>& deps_):
        name(name_), deps(deps_)
    {
    }

    const std::string& getName() const override
    {
        return name;
    }

    const std::list<std::string>& getDependencies() const override
    {
        return deps;
    }

private:
    std::string name;
    std::list<std::string> deps;
};


class TestFilterHeader: public TestFilter
{
public:
    TestFilterHeader():
        TestFilter("test-header", {})
    {
    }

    void filter(Phase phase, MessageContext* context) override
    {
        NGREST_DEBUG_ASSERT(phase == Phase::Header, "invalid phase"); // only for test. should never happen

        for (Header* header = context->request->headers; header; header = header->next)
            LogDebug() << "header: " << header->name << " : " << header->value;

        // throw HttpException to stop processing request and close connection
        if (context->request->getHeader("x-test-header-throw"))
            NGREST_THROW_HTTP("Throw found in headers", HTTP_STATUS_418_IM_A_TEAPOT);

        if (context->request->getHeader("x-test-header")) {
            NGREST_ASSERT_NULL(context->request->path);
            std::string newPath = context->request->path;
            stringReplace(newPath, "0", "_ZERO_", true);
            context->request->path = context->pool->putCString(newPath.data(), newPath.size(), true);
        }
    }
};

class TestFilterHeader2: public TestFilter
{
public:
    TestFilterHeader2():
        TestFilter("test-header2", {"test-header"})
    {
    }

    void filter(Phase phase, MessageContext* context) override
    {
        NGREST_DEBUG_ASSERT(phase == Phase::Header, "invalid phase"); // only for test. should never happen

        // throw HttpException to stop processing request and close connection
        if (context->request->getHeader("x-test-header-throw2"))
            NGREST_THROW_HTTP("Throw2 found in headers", HTTP_STATUS_418_IM_A_TEAPOT);
    }
};

class TestFilterPreDispatch: public TestFilter
{
public:
    TestFilterPreDispatch():
        TestFilter("test-predispatch", {})
    {
    }

    void filter(Phase phase, MessageContext* context) override
    {
        NGREST_DEBUG_ASSERT(phase == Phase::PreDispatch, "invalid phase"); // only for test. should never happen

        if (context->request->getHeader("x-test-predispatch-throw"))
            NGREST_THROW_HTTP("Throw found in headers", HTTP_STATUS_418_IM_A_TEAPOT);

        // replace if appropriate HTTP header is set
        if (!context->request->getHeader("x-test-predispatch"))
            return;

        if (!context->request->body) { // GET, DELETE
            // replace 1 to _ONE_ in request url
            // it's usually small, so we just allocate new buffer for it
            NGREST_ASSERT_NULL(context->request->path);
            std::string newPath = context->request->path;
            stringReplace(newPath, "1", "_ONE_", true);
            context->request->path = context->pool->putCString(newPath.data(), newPath.size(), true);
        } else {  // POST, PUT
            // body is stored in external memory pool
            // it may be large, so try mo miminize memory consumption here

            char* request = context->request->body;
            uint64_t size = context->request->bodySize;
            char* requestEnd = request + size;
            char* pos = std::find(request, requestEnd, '1');
            if (pos != requestEnd) {
                // replace 1 to _ONE_ for the test

                // store occurrende offset
                uint64_t offset = pos - request;

                // pool body can be null in case of small requests which has been readed at once
                // use another pool then
                if (!context->request->poolBody) {
                    // small buffer mode
                    char* newBuff = context->pool->grow(context->request->bodySize + 4);
                    memcpy(newBuff, context->request->body, offset);
                    memcpy(newBuff + offset, "_ONE_", 5);
                    memcpy(newBuff + offset + 5, context->request->body + offset + 1, context->request->bodySize - offset - 1);
                    context->request->body = newBuff;
                    context->request->bodySize += 4;
                } else {
                    // large body pool mode

                    // increate request buffer by 4
                    context->request->poolBody->grow(4);
                    MemPool::Chunk* chunk = context->request->poolBody->flatten();
                    // request ptr and body ptr are invalid after flatten
                    // update body size
                    request = chunk->buffer;

                    // shift data after '1' to 4 bytes to the right
                    memmove(request + offset + 5, request + offset + 1, context->request->bodySize - offset - 1);
                    // place _ONE_ into buffer offset
                    memcpy(request + offset, "_ONE_", 5);

                    context->request->body = chunk->buffer;
                    context->request->bodySize = chunk->size;
                }
            }

            // when changing request memory pool need to call flattern() and update
            // context->request->body and context->request->bodySize
        }
    }
};

class TestFilterPreInvoke: public TestFilter
{
public:
    TestFilterPreInvoke():
        TestFilter("test-preinvoke", {})
    {
    }

    void filter(Phase phase, MessageContext* context) override
    {
        NGREST_DEBUG_ASSERT(phase == Phase::PreInvoke, "invalid phase"); // only for test. should never happen

        if (context->request->getHeader("x-test-preinvoke-throw"))
            NGREST_THROW_HTTP("Throw found in headers", HTTP_STATUS_418_IM_A_TEAPOT);

        // replace if appropriate HTTP header is set
        if (!context->request->getHeader("x-test-preinvoke"))
            return;

        NGREST_ASSERT_HTTP(context->request->node && context->request->node->type == NodeType::Object,
                           HTTP_STATUS_400_BAD_REQUEST, "Invalid request node type");


        Object* obj = static_cast<Object*>(context->request->node);
        NamedNode* valueNode = obj->firstChild;
        NGREST_ASSERT_HTTP(valueNode, HTTP_STATUS_400_BAD_REQUEST, "can't find node value");

        NGREST_ASSERT_HTTP(valueNode->node && valueNode->node->type == NodeType::Value,
                           HTTP_STATUS_400_BAD_REQUEST, "Unexpected node type");

        Value* val = static_cast<Value*>(valueNode->node);
        std::string newVal = val->value;

        stringReplace(newVal, "2", "33", true);

        val->value = context->pool->putCString(newVal.c_str(), newVal.size(), true);
    }
};

class TestFilterPostDispatch: public TestFilter
{
public:
    TestFilterPostDispatch():
        TestFilter("test-postdispatch", {})
    {
    }

    void filter(Phase phase, MessageContext* context) override
    {
        NGREST_DEBUG_ASSERT(phase == Phase::PostDispatch, "invalid phase"); // only for test. should never happen

        if (context->request->getHeader("x-test-postdispatch-throw"))
            NGREST_THROW_HTTP("Throw found in headers", HTTP_STATUS_418_IM_A_TEAPOT);

        // replace if appropriate HTTP header is set
        if (!context->request->getHeader("x-test-postdispatch"))
            return;

        NGREST_ASSERT_HTTP(context->response->node && context->response->node->type == NodeType::Object,
                           HTTP_STATUS_400_BAD_REQUEST, "Invalid response node type");


        Object* obj = static_cast<Object*>(context->response->node);
        NamedNode* valueNode = obj->firstChild;
        NGREST_ASSERT_HTTP(valueNode, HTTP_STATUS_400_BAD_REQUEST, "can't find node value");

        NGREST_ASSERT_HTTP(valueNode->node && valueNode->node->type == NodeType::Value,
                           HTTP_STATUS_400_BAD_REQUEST, "Unexpected node type");

        Value* val = static_cast<Value*>(valueNode->node);
        std::string newVal = val->value;

        stringReplace(newVal, "3", "44", true);

        val->value = context->pool->putCString(newVal.c_str(), newVal.size(), true);

        // put additional header for the test
        context->response->headers = context->pool->alloc<Header>("x-test-postdispatch", "ok",
                                                                  context->response->headers);
    }
};

class TestFilterPreSend: public TestFilter
{
public:
    TestFilterPreSend():
        TestFilter("test-presend", {})
    {
    }

    void filter(Phase phase, MessageContext* context) override
    {
        NGREST_DEBUG_ASSERT(phase == Phase::PreSend, "invalid phase"); // only for test. should never happen

        if (context->request->getHeader("x-test-presend-throw"))
            NGREST_THROW_HTTP("Throw found in headers", HTTP_STATUS_418_IM_A_TEAPOT);

        // replace if appropriate HTTP header is set
        if (!context->request->getHeader("x-test-presend"))
            return;

        // replace first 4 in response to *
        // scan output pool for occurrence
        int chunkCount = context->response->poolBody->getChunkCount();
        const MemPool::Chunk* chunk = context->response->poolBody->getChunks();
        for (int i = 0; i < chunkCount; ++i, ++chunk) {
             char* pos = std::find(chunk->buffer, chunk->buffer + chunk->size, '4');
             if (pos != (chunk->buffer + chunk->size)) {
                 *pos = '*';
                 break;
             }
        }
    }
};


TestFilterGroup::TestFilterGroup():
  filters({
      {Phase::Header, {new TestFilterHeader2(), new TestFilterHeader()}}, // test dep
      {Phase::PreDispatch, {new TestFilterPreDispatch()}},
      {Phase::PreInvoke, {new TestFilterPreInvoke()}},
      {Phase::PostDispatch, {new TestFilterPostDispatch()}},
      {Phase::PreSend, {new TestFilterPreSend()}},
  })
{
}

TestFilterGroup::~TestFilterGroup()
{
    for (auto it : filters)
        for (Filter* filter : it.second)
            delete filter;
    filters.clear();
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
