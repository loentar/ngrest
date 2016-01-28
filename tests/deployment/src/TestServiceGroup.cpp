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

#include <ngrest/engine/ServiceGroup.h>

#include "TestDeploymentWrapper.h"
#include "TestServiceGroup.h"

namespace ngrest {

TestServiceGroup::TestServiceGroup():
    name("test")
{
    services = {{
        new ::ngrest::TestDeploymentWrapper()
    }};
}

TestServiceGroup::~TestServiceGroup()
{
    for (ServiceWrapper* service : services)
        delete service;
    services.clear();
}

const std::string& TestServiceGroup::getName() const
{
    return name;
}

const std::vector<ServiceWrapper*>& TestServiceGroup::getServices() const
{
    return services;
}

}
