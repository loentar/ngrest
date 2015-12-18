#ifndef NGREST_DEPLOYMENT_H
#define NGREST_DEPLOYMENT_H

namespace ngrest {

class ServiceWrapper;
class Node;
class MessageContext;

class Deployment
{
public:
    Deployment();
    ~Deployment();

    void registerService(ServiceWrapper* wrapper);
    void unregisterService(ServiceWrapper* wrapper);

    void dispatchMessage(MessageContext* context);

private:
    struct Impl;
    Impl* const impl;
};

} // namespace ngrest

#endif // NGREST_DEPLOYMENT_H
