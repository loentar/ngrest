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

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/Log.h>

#include "Phase.h"
#include "Filter.h"
#include "FilterDispatcher.h"

namespace ngrest {

struct FilterDispatcher::Impl
{
    std::list<Filter*> filtersByPhase[static_cast<int>(Phase::Count)];

    inline std::list<Filter*>& filters(Phase phase)
    {
        return filtersByPhase[static_cast<int>(phase)];
    }

    bool registerOneFilter(std::list<Filter*>& filtersOut, Filter* filter)
    {
        NGREST_ASSERT_NULL(filter);
        const std::list<std::string>& inDeps = filter->getDependencies();
        if (inDeps.empty()) { // add filters without dependencies to front
            filtersOut.push_front(filter);
            return true;
        }

        // check filter dependencies and push filter to the end in case of deps are ok
        for (const std::string& dep : inDeps) {
            bool found = false;
            for (const Filter* filterOut : filtersOut) {
                if (filterOut->getName() == dep) {
                    found = true;
                    break;
                }
            }

            if (!found)
                return false;
        }

        filtersOut.push_back(filter);
        return true;
    }

    bool unregisterOneFilter(std::list<Filter*>& filtersOut, Filter* filter, bool withDeps)
    {
        NGREST_ASSERT_NULL(filter);

        auto end = filtersOut.end();
        auto it = std::find(filtersOut.begin(), end, filter);
        if (it == end) // not found
            return false;

        const std::list<std::string>& inDeps = filter->getDependencies();
        if (inDeps.empty()) { // remove filter without dependencies
            filtersOut.erase(it);
            return true;
        }


        const std::string& filterName = filter->getName();
        // check references to this filter

        while (it != end) {
            const auto& deps = (*it)->getDependencies();
            ++it;
            if (std::find(deps.begin(), deps.end(), filterName) != deps.end())
                unregisterOneFilter(filtersOut, *it, withDeps);
        }

        return true;
    }
};

FilterDispatcher::FilterDispatcher():
    impl(new Impl())
{
}

FilterDispatcher::~FilterDispatcher()
{
    delete impl;
}

void FilterDispatcher::registerFilters(Phase phase, std::list<Filter*> filtersIn)
{
    std::list<Filter*>& filtersOut = impl->filters(phase);
    bool changed;
    do {
        changed = false;
        for (auto itIn = filtersIn.begin(); itIn != filtersIn.end();) {
            Filter* filter = *itIn;
            if (impl->registerOneFilter(filtersOut, filter)) {
                itIn = filtersIn.erase(itIn);
                changed = true;
            } else {
                ++itIn;
            }
        }
    } while (changed);

    if (!filtersIn.empty()) { // unmet deps
        std::string unmetDeps;
        for (const Filter* filter : filtersIn) {
            unmetDeps += "\n  " + filter->getName() + " => ";
            for (const std::string& dep : filter->getDependencies())
                unmetDeps += dep + " ";
        }

        NGREST_THROW_ASSERT("Some filters has unmet dependencies:" + unmetDeps);
    }
}

bool FilterDispatcher::registerFilter(Phase phase, Filter* filter)
{
    return impl->registerOneFilter(impl->filters(phase), filter);
}

bool FilterDispatcher::unregisterFilter(Phase phase, Filter* filter, bool withDeps)
{
    return impl->unregisterOneFilter(impl->filters(phase), filter, withDeps);
}

void FilterDispatcher::processFilters(Phase phase, MessageContext* context)
{
    NGREST_ASSERT_PARAM(context);
    for (Filter* filter : impl->filters(phase)) {
#ifdef DEBUG
        LogDebug() << "Running filter " << filter->getName()
                   << " for phase " << PhaseInfo::phaseToString(phase);
#endif
        filter->filter(phase, context);
    }
}

std::list<Filter*> FilterDispatcher::getFilters(Phase phase) const
{
    return impl->filters(phase);
}

Filter* FilterDispatcher::getFilter(Phase phase, const std::string& name) const
{
    for (auto filter : impl->filters(phase)) {
        if (filter->getName() == name) {
            return filter;
        }
    }

    return nullptr;
}


} // namespace ngrest
