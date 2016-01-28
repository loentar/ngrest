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

namespace ngrest {

class ServiceWrapper;
class MessageContext;

class ServiceDispatcher
{
public:
    ServiceDispatcher();
    ~ServiceDispatcher();

    void registerService(ServiceWrapper* wrapper);
    void unregisterService(ServiceWrapper* wrapper);

    void dispatchMessage(MessageContext* context);

    std::vector<ServiceWrapper*> getServices() const;

    ServiceWrapper* getService(const std::string& name) const;

private:
    struct Impl;
    Impl* const impl;
};

} // namespace ngrest

#endif // NGREST_SERVICEDISPATCHER_H
