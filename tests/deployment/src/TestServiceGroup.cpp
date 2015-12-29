#include <ngrest/engine/ServiceGroup.h>

#include "TestDeploymentWrapper.h"
#include "TestServiceGroup.h"

namespace ngrest {

TestServiceGroup::TestServiceGroup()
{
    services = {{
        new ::ngrest::TestDeploymentWrapper()
    }};
}

TestServiceGroup::~TestServiceGroup()
{
    for (auto it = services.begin(); it != services.end(); ++it)
        delete *it;
    services.clear();
}

std::string TestServiceGroup::getName()
{
    return "test";
}

const std::vector<ServiceWrapper*>& TestServiceGroup::getServices()
{
    return services;
}

}
