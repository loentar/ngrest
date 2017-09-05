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

#ifndef NGREST_SERVICEGROUP_H
#define NGREST_SERVICEGROUP_H

#include <string>
#include <vector>
#include "ngrestengineexport.h"

namespace ngrest {

class ServiceWrapper;

/**
 * @brief Group of services to deploy from shared library
 */
class NGREST_ENGINE_EXPORT ServiceGroup
{
public:
    ServiceGroup();
    virtual ~ServiceGroup();

    /**
     * @brief gets name of service group
     * @return name of service group
     */
    virtual const std::string& getName() const = 0;

    /**
     * @brief get list of service wrappers provided by group
     * @return list of service wrappers
     */
    virtual const std::vector<ServiceWrapper*>& getServices() const = 0;
};

} // namespace ngrest

#endif // NGREST_SERVICEGROUP_H
