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
#include <ngrest/utils/Log.h>
#include <ngrest/common/Message.h>

#include "ServiceDispatcher.h"
#include "Transport.h"
#include "Engine.h"

namespace ngrest {

// replace callback to write response with appropriate transport
class EngineHookCallback: public MessageCallback
{
public:
    EngineHookCallback(MessageContext* context_):
        context(context_), origCallback(context_->callback)
    {
        context->callback = this;
    }

    void success()
    {
        // only write response in case of it was not written
        if (!context->response->poolBody->getSize())
            context->transport->writeResponse(context->pool, context->request, context->response);
        context->callback = origCallback;
        context->callback->success();
    }

    void error(const Exception& error)
    {
        context->callback = origCallback;
        context->callback->error(error);
    }

    MessageContext* const context;
    MessageCallback* const origCallback;
};


Engine::Engine(ServiceDispatcher& dispatcher_):
    dispatcher(dispatcher_)
{
}

void Engine::dispatchMessage(MessageContext* context)
{
    NGREST_ASSERT_PARAM(context);
    NGREST_ASSERT_NULL(context->request);
    NGREST_ASSERT_NULL(context->response);
    NGREST_ASSERT_NULL(context->callback);

    try {
        // this will replace context callback and restore it after dispatching the message
        context->pool->alloc<EngineHookCallback>(context);

        if (context->request->body) {
            context->request->node = context->transport->parseRequest(context->pool, context->request);
            NGREST_ASSERT(context->request->node, "Failed to read request"); // should never throw
        }

        dispatcher.dispatchMessage(context);
    } catch (const Exception& err) {
        LogWarning() << err.getFileLine() << " " << err.getFunction() << " : " << err.what();
        context->callback->error(err);
    }
}

ServiceDispatcher& Engine::getDispatcher()
{
    return dispatcher;
}

} // namespace ngrest

