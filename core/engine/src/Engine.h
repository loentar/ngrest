#ifndef NGREST_ENGINE_H
#define NGREST_ENGINE_H

namespace ngrest {

struct MessageContext;
class Deployment;

class Engine
{
public:
    Engine(Deployment& deployment);

    void dispatchMessage(MessageContext* context);

private:
    Deployment& deployment;
};

} // namespace ngrest

#endif // NGREST_ENGINE_H
