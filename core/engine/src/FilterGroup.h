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

#ifndef NGREST_FILTERGROUP_H
#define NGREST_FILTERGROUP_H

#include <string>
#include <unordered_map>
#include <list>

#include <ngrest/utils/EnumClassHash.h>

#include "ngrestengineexport.h"

namespace ngrest {

enum class Phase;
class Filter;

typedef std::unordered_map<Phase, std::list<Filter*>, EnumClassHash> FiltersMap;

/**
 * @brief Group of filters to deploy from shared library
 */
class NGREST_ENGINE_EXPORT FilterGroup
{
public:
    FilterGroup();
    virtual ~FilterGroup();

    /**
     * @brief gets name of filter group
     * @return name of filter group
     */
    virtual const std::string& getName() const = 0;

    /**
     * @brief get list of filter provided by group
     * @return list of filter
     */
    virtual const FiltersMap& getFilters() const = 0;
};

} // namespace ngrest

#endif // NGREST_FILTERGROUP_H
