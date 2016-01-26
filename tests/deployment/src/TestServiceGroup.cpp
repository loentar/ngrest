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
