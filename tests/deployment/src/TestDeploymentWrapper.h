#ifndef NGREST_TESTDEPLOYMENTWRAPPER_H
#define NGREST_TESTDEPLOYMENTWRAPPER_H

#include <ngrest/engine/ServiceWrapper.h>

namespace ngrest {

class TestDeployment;

class TestDeploymentWrapper: public ServiceWrapper
{
public:
    TestDeploymentWrapper();
    ~TestDeploymentWrapper();

    virtual Service* serviceImpl() override;
    virtual void invoke(const OperationDescription* operation, MessageContext* context) override;
    virtual const ServiceDescription* description() override;

private:
    TestDeployment* service;
};

} // namespace ngrest

#endif // NGREST_TESTDEPLOYMENTWRAPPER_H
