#ifndef NGREST_SERVICEDISPATCHER_H
#define NGREST_SERVICEDISPATCHER_H

#include <vector>

namespace ngrest {

class ServiceWrapper;
class MessageContext;

class ServiceDispatcher
{
public:
    ServiceDispatcher();
    ~ServiceDispatcher();

    void registerService(ServiceWrapper* wrapper);
    void unregisterService(ServiceWrapper* wrapper);

    void dispatchMessage(MessageContext* context);

    std::vector<ServiceWrapper*> getServices() const;

    ServiceWrapper* getService(const std::string& name) const;

private:
    struct Impl;
    Impl* const impl;
};

} // namespace ngrest

#endif // NGREST_SERVICEDISPATCHER_H
