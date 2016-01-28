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

#ifndef NGREST_SERVICEWRAPPER_H
#define NGREST_SERVICEWRAPPER_H

namespace ngrest {

class Service;
class ServiceDescription;
class OperationDescription;
class MessageContext;

class ServiceWrapper
{
public:
    ServiceWrapper();
    virtual ~ServiceWrapper();

    virtual Service* getServiceImpl() = 0;

    virtual void invoke(const OperationDescription* operation, MessageContext* context) = 0;

    // server side service description
    virtual const ServiceDescription* getDescription() const = 0;
};

} // namespace ngrest

#endif // NGREST_SERVICEWRAPPER_H
