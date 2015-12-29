#ifndef NGREST_TESTSERVICEGROUP_H
#define NGREST_TESTSERVICEGROUP_H

#include <ngrest/engine/ServiceGroup.h>

namespace ngrest {

class TestServiceGroup: public ServiceGroup
{
public:
    TestServiceGroup();

    ~TestServiceGroup();

    std::string getName() override;

    const std::vector<ServiceWrapper*>& getServices() override;

private:
    std::vector<ServiceWrapper*> services;
};

}


#endif // NGREST_TESTSERVICEGROUP_H
