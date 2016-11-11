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

#ifndef NGREST_FILTERDISPATCHER_H
#define NGREST_FILTERDISPATCHER_H

#include <string>
#include <list>
#include "Phase.h"
#include "ngrestengineexport.h"

namespace ngrest {

class Filter;
struct MessageContext;

/**
 * @brief filter dispatcher
 * manages the filters and dispatches message through filters
 */
class NGREST_ENGINE_EXPORT FilterDispatcher
{
public:
    FilterDispatcher();
    ~FilterDispatcher();

    /**
     * @brief resolve dependencies and register filters for given phase
     * @param phase filter phase
     * @param filters filters to register
     */
    void registerFilters(Phase phase, std::list<Filter*> filters);

    /**
     * @brief register filter checking dependencies
     * @param phase filter phase
     * @param filter filter
     * throws dependence exception when filter has unmet dependencies
     */
    bool registerFilter(Phase phase, Filter* filter);

    /**
     * @brief unregister filter checking dependencies
     * @param phase filter phase
     * @param filter filter
     * @param withDeps true - unregister filters which depends on this filter too
     *   false - throw dependence exception if other filters depends on this
     */
    bool unregisterFilter(Phase phase, Filter* filter, bool withDeps = true);


    /**
     * @brief process messages throught the filters
     * @param phase phase to process
     * @param context message to process
     */
    void processFilters(Phase phase, MessageContext* context);


    /**
     * @brief get all registered filters
     * @param phase filter phase
     * @return list of filters registered for given phase
     */
    std::list<Filter*> getFilters(Phase phase) const;


    /**
     * @brief get filter by name
     * @param phase messsage dispatch phase
     * @param name filter name
     * @return pointer to filter or nullptr if no filter found
     */
    Filter* getFilter(Phase phase, const std::string& name) const;

private:
    FilterDispatcher(const FilterDispatcher&);
    FilterDispatcher& operator=(const FilterDispatcher&);

private:
    struct Impl;
    Impl* const impl;
};

} // namespace ngrest

#endif // NGREST_FILTERDISPATCHER_H
