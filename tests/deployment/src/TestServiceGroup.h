#ifndef NGREST_TESTSERVICEGROUP_H
#define NGREST_TESTSERVICEGROUP_H

#include <ngrest/engine/ServiceGroup.h>

namespace ngrest {

class TestServiceGroup: public ServiceGroup
{
public:
    TestServiceGroup();

    ~TestServiceGroup();

    const std::string& getName() const override;

    const std::vector<ServiceWrapper*>& getServices() const override;

private:
    const std::string name;
    std::vector<ServiceWrapper*> services;
};

}


#endif // NGREST_TESTSERVICEGROUP_H
