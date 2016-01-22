#ifndef NGREST_ENGINE_H
#define NGREST_ENGINE_H

namespace ngrest {

struct MessageContext;
class ServiceDispatcher;

class Engine
{
public:
    Engine(ServiceDispatcher& dispatcher);

    void dispatchMessage(MessageContext* context);

    ServiceDispatcher& getDispatcher();

private:
    ServiceDispatcher& dispatcher;
};

} // namespace ngrest

#endif // NGREST_ENGINE_H
