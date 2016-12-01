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

#ifndef NGREST_SERVICEDISPATCHER_H
#define NGREST_SERVICEDISPATCHER_H

#include <vector>
#include "ngrestengineexport.h"

namespace ngrest {

class ServiceWrapper;
struct MessageContext;

/**
 * @brief service dispatcher
 * manages the services and dispatches message to services
 */
class NGREST_ENGINE_EXPORT ServiceDispatcher
{
public:
    /**
     * @brief constructor
     */
    ServiceDispatcher();

    /**
     * @brief destructor
     */
    ~ServiceDispatcher();


    /**
     * @brief register service
     * @param wrapper service wrapper
     */
    void registerService(ServiceWrapper* wrapper);

    /**
     * @brief unregister service
     * @param wrapper service wrapper
     */
    void unregisterService(ServiceWrapper* wrapper);


    /**
     * @brief dispatch message to the service
     * @param context message
     */
    void dispatchMessage(MessageContext* context);


    /**
     * @brief get all registered services
     * @return list of services registered
     */
    std::vector<ServiceWrapper*> getServices() const;


    /**
     * @brief get service by name
     * @param name service name
     * @return pointer to service wrapper or nullptr if no service found
     */
    ServiceWrapper* getService(const std::string& name) const;

private:
    ServiceDispatcher(const ServiceDispatcher&);
    ServiceDispatcher& operator=(const ServiceDispatcher&);

private:
    struct Impl;
    Impl* const impl;
};

} // namespace ngrest

#endif // NGREST_SERVICEDISPATCHER_H
