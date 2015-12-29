#ifndef NGREST_SERVICEDISPATCHER_H
#define NGREST_SERVICEDISPATCHER_H

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

private:
    struct Impl;
    Impl* const impl;
};

} // namespace ngrest

#endif // NGREST_SERVICEDISPATCHER_H
