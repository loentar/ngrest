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
    for (auto it = services.begin(); it != services.end(); ++it)
        delete *it;
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
