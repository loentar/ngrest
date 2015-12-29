#ifndef NGREST_DEPLOYMENT_H
#define NGREST_DEPLOYMENT_H

#include <string>

namespace ngrest {

class ServiceDispatcher;
class ServiceGroup;

class Deployment
{
public:
    Deployment(ServiceDispatcher& dispatcher);
    ~Deployment();

    void deployAll();

    void deploy(const std::string& servicePath);
    void undeploy(const std::string& servicePath);

    void deployStatic(ServiceGroup* serviceGroup);
    void undeployStatic(ServiceGroup* serviceGroup);

private:
    class Impl;
    Impl* impl;
};

} // namespace ngrest

#endif // NGREST_DEPLOYMENT_H
