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

#ifndef NGREST_ENGINE_H
#define NGREST_ENGINE_H

#include "ngrestengineexport.h"

namespace ngrest {

enum class Phase;
struct MessageContext;
class ServiceDispatcher;
class FilterDispatcher;

/**
 * @brief Message processing engine.
 * - parse body of request using supplied transport parser to OM
 * - dispatch message using service dispatcher
 * - write response and pass it to the transport
 */
class NGREST_ENGINE_EXPORT Engine
{
public:
    /**
     * @brief constructs engine with given dispatcher
     * @param serviceDispatcher service dispatcher to use
     */
    Engine(ServiceDispatcher& serviceDispatcher);

    /**
     * @brief set filter dispatcher for engine
     * @param filterDispatcher filter dispatcher to use
     */
    void setFilterDispatcher(FilterDispatcher* filterDispatcher);

    /**
     * @brief run phase for the message
     * @param phase message phase
     * @param context message context
     */
    void runPhase(Phase phase, MessageContext* context);

    /**
     * @brief parse, dispatch message and write response
     * @param context message context
     */
    void dispatchMessage(MessageContext* context);

    /**
     * @brief get service dispatcher
     * @return service dispatcher
     */
    ServiceDispatcher& getServiceDispatcher();

    /**
     * @brief get filter dispatcher
     * @return filter dispatcher
     */
    FilterDispatcher* getFilterDispatcher();

private:
    ServiceDispatcher& serviceDispatcher;
    FilterDispatcher* filterDispatcher = nullptr;
};

} // namespace ngrest

#endif // NGREST_ENGINE_H
