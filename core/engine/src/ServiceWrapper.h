#ifndef NGREST_SERVICEWRAPPER_H
#define NGREST_SERVICEWRAPPER_H

namespace ngrest {

class Service;
class ServiceDescription;
class OperationDescription;
class MessageContext;

class ServiceWrapper
{
public:
    ServiceWrapper();
    virtual ~ServiceWrapper();

    virtual Service* getServiceImpl() = 0;

    virtual void invoke(const OperationDescription* operation, MessageContext* context) = 0;

    // server side service description
    virtual const ServiceDescription* getDescription() const = 0;
};

} // namespace ngrest

#endif // NGREST_SERVICEWRAPPER_H
