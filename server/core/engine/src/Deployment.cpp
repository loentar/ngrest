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

#include <core/utils/File.h>
#include <core/utils/Log.h>
#include <core/utils/Plugin.h>
#include <ngrest/engine/ServiceDescription.h>

#include "ServiceGroup.h"
#include "ServiceWrapper.h"
#include "Deployment.h"
#include "ServiceDispatcher.h"

namespace ngrest {

class Deployment::Impl
{
public:
    std::unordered_map<std::string, Plugin<ServiceGroup>> serviceLibs;
    ServiceDispatcher& dispatcher;

    Impl(ServiceDispatcher& dispatcher_):
        dispatcher(dispatcher_)
    {
    }

};

Deployment::Deployment(ServiceDispatcher& dispatcher):
    impl(new Impl(dispatcher))
{
}

Deployment::~Deployment()
{
    delete impl;
}

void Deployment::deployAll(const std::string& servicesPath)
{
    // find service libraries
    StringList libs;
    File(servicesPath).list(libs, "*" NGREST_LIBRARY_EXT, File::AttributeAnyFile);

    if (libs.empty()) {
        LogError() << "No services found";
        return;
    }

    for (const std::string& lib : libs) {
        const std::string& servicePath = servicesPath + lib;
        try {
            deploy(servicePath);
        } catch (const Exception& exception) {
            LogWarning() << "Can't load service: " << servicePath << ": " << exception.what();
        } catch (...) {
            LogWarning() << "Can't load service: " << servicePath << ": unknown error";
        }
    }
}

void Deployment::deploy(const std::string& servicePath)
{
    // load service library
    LogDebug() << "Deploying service library: " << servicePath << "...";

    Plugin<ServiceGroup> serviceLib;
    serviceLib.load(servicePath);

    ServiceGroup* serviceGroup = serviceLib.getPluginSymbol();
    NGREST_ASSERT_NULL(serviceGroup); // should never happen

    deployStatic(serviceGroup);

    impl->serviceLibs[servicePath] = serviceLib;
}

void Deployment::undeploy(const std::string& servicePath)
{
    auto existing = impl->serviceLibs.find(servicePath);
    NGREST_ASSERT(existing != impl->serviceLibs.end(), "Cannot undeploy service lib ["
                  + servicePath + "]: not deployed");

    ServiceGroup* serviceGroup = existing->second.getPluginSymbol();
    NGREST_ASSERT_NULL(serviceGroup); // should never happen

    undeployStatic(serviceGroup);

    impl->serviceLibs.erase(existing);
}

void Deployment::undeployAll()
{
    for (auto& serviceLib : impl->serviceLibs) {
        undeployStatic(serviceLib.second.getPluginSymbol());
    }
}

void Deployment::deployStatic(ServiceGroup* serviceGroup)
{
    LogDebug() << "Deploying service group name: " << serviceGroup->getName();

    const std::vector<ServiceWrapper*>& serviceWrappers = serviceGroup->getServices();

    if (serviceWrappers.empty()) {
        LogError() << "Failed to deploy [" << serviceGroup->getName() << "]. No services.";
        return;
    }

    auto it = serviceWrappers.begin();
    try {
        for (; it != serviceWrappers.end(); ++it)
            impl->dispatcher.registerService(*it);
    } catch (...) {
        // unregister services in case of error
        for (; it != serviceWrappers.begin(); --it) {
            try {
                impl->dispatcher.unregisterService(*it);
            } NGREST_CATCH_ALL;
        }

        LogError() << "Failed to deploy [" << serviceGroup->getName() << "].";

        throw;
    }
}

void Deployment::undeployStatic(ServiceGroup* serviceGroup)
{
    LogDebug() << "Undeploying service group name: " << serviceGroup->getName();

    const std::vector<ServiceWrapper*>& serviceWrappers = serviceGroup->getServices();

    if (serviceWrappers.empty()) {
        LogError() << "Failed to deploy [" << serviceGroup->getName() << "]. No services.";
        return;
    }

    for (ServiceWrapper* wrapper : serviceWrappers) {
        try {
            impl->dispatcher.unregisterService(wrapper);
        } NGREST_CATCH_ALL;
    }
}

} // namespace ngrest
