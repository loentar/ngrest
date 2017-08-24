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

#include <unordered_map>

#include <ngrest/utils/File.h>
#include <ngrest/utils/Log.h>
#include <ngrest/utils/Plugin.h>

#include "FilterDeployment.h"
#include "FilterDispatcher.h"
#include "FilterGroup.h"
#include "LatestLibs.h"

namespace ngrest {

class FilterDeployment::Impl
{
public:
    std::unordered_map<std::string, Plugin<FilterGroup>> filterLibs;
    FilterDispatcher& filterDispatcher;

    Impl(FilterDispatcher& filterDispatcher_):
        filterDispatcher(filterDispatcher_)
    {
    }

};

FilterDeployment::FilterDeployment(FilterDispatcher& filterDispatcher):
    impl(new Impl(filterDispatcher))
{
}

FilterDeployment::~FilterDeployment()
{
    delete impl;
}

void FilterDeployment::deployAll(const std::string& filtersPath, FilterDeployment* oldDeployment)
{
    // find filter libraries
    StringList libs;
    File(filtersPath).list(libs, "*" NGREST_LIBRARY_EXT, File::AttributeAnyFile);

    LatestLibs latestLibs(libs);

    if (latestLibs.map.empty()) {
        LogDebug() << "No filters found";
        return;
    }

    auto* oldLibs = oldDeployment ? &oldDeployment->impl->filterLibs : 0;

    for (const auto& lib : latestLibs.map) {
        const std::string& filterPath = filtersPath + lib.second.filename;
        try {
            if (oldLibs) {
                const auto iterOldLib = oldLibs->find(filterPath);
                if (iterOldLib != oldLibs->end())
                    impl->filterLibs[filterPath] = std::move(iterOldLib->second);
                else
                    deploy(filterPath);
            }
            else
                deploy(filterPath);
        } catch (const Exception& exception) {
            LogWarning() << "Can't load filter: " << filterPath << ": " << exception.what();
        } catch (...) {
            LogWarning() << "Can't load filter: " << filterPath << ": unknown error";
        }
    }
}

void FilterDeployment::deploy(const std::string& filterPath)
{
    // load filter library
    LogDebug() << "Deploying filter library: " << filterPath << "...";

    Plugin<FilterGroup> filterLib;
    filterLib.load(filterPath);

    FilterGroup* filterGroup = filterLib.getPluginSymbol();
    NGREST_ASSERT_NULL(filterGroup); // should never happen

    deployStatic(filterGroup);

    impl->filterLibs[filterPath] = std::move(filterLib);
}

void FilterDeployment::undeploy(const std::string& filterPath)
{
    auto existing = impl->filterLibs.find(filterPath);
    NGREST_ASSERT(existing != impl->filterLibs.end(), "Cannot undeploy filter lib ["
                  + filterPath + "]: not deployed");

    FilterGroup* filterGroup = existing->second.getPluginSymbol();
    NGREST_ASSERT_NULL(filterGroup); // should never happen

    undeployStatic(filterGroup);

    impl->filterLibs.erase(existing);
}

void FilterDeployment::undeployAll()
{
    for (auto& filterLib : impl->filterLibs) {
        undeployStatic(filterLib.second.getPluginSymbol());
    }
}

void FilterDeployment::deployStatic(FilterGroup* filterGroup)
{
    LogDebug() << "Deploying filter group: " << filterGroup->getName();

    const FiltersMap& filtersByPhase = filterGroup->getFilters();

    if (filtersByPhase.empty()) {
        LogError() << "Failed to deploy [" << filterGroup->getName() << "]. No filters.";
        return;
    }

    auto itPhase = filtersByPhase.begin();
    for (; itPhase != filtersByPhase.end(); ++itPhase) {
        try {
            impl->filterDispatcher.registerFilters(itPhase->first, itPhase->second);
        } catch (...) {
            // unregister filters in case of error
            for (auto itPhase1 = filtersByPhase.begin(); itPhase1 != filtersByPhase.end(); ++itPhase1) {
                const std::list<Filter*>& filters = itPhase1->second;
                for (Filter* filter : filters) {
                    try {
                        impl->filterDispatcher.unregisterFilter(itPhase1->first, filter, false);
                    } NGREST_CATCH_ALL;
                }

                if (itPhase1 == itPhase)
                    break;
            }

            LogError() << "Failed to deploy [" << filterGroup->getName() << "].";

            throw;
        }
    }
}

void FilterDeployment::undeployStatic(FilterGroup* filterGroup)
{
    LogDebug() << "Undeploying filter group: " << filterGroup->getName();

    const FiltersMap& filters = filterGroup->getFilters();

    if (filters.empty()) {
        LogError() << "Failed to deploy [" << filterGroup->getName() << "]. No filters.";
        return;
    }

    for (auto itPhase = filters.begin(); itPhase != filters.end(); ++itPhase) {
        const std::list<Filter*>& filters = itPhase->second;
        for (Filter* filter : filters) {
            try {
                impl->filterDispatcher.unregisterFilter(itPhase->first, filter, false);
            } NGREST_CATCH_ALL;
        }
    }
}

} // namespace ngrest
