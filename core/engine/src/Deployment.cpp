#include <unordered_map>

#include <ngrest/utils/File.h>
#include <ngrest/utils/Log.h>
#include <ngrest/utils/Plugin.h>
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

void Deployment::deployAll()
{
    // find service libraries
    StringList libs;
    const std::string& servicesPath = "services";
    File(servicesPath).list(libs, "*" NGREST_LIBRARY_EXT, File::AttributeRegularFile);

    if (servicesPath.empty()) {
        LogError() << "No services found";
        return;
    }

    for (StringList::const_iterator it = libs.begin(); it != libs.end(); ++it) {
        const std::string& servicePath = servicesPath + *it;
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

    for (auto it = serviceWrappers.begin(); it != serviceWrappers.end(); ++it) {
        try {
            impl->dispatcher.unregisterService(*it);
        } NGREST_CATCH_ALL;
    }
}
} // namespace ngrest
