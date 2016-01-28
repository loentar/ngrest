#ifndef NGREST_SERVERSTATUS_H
#define NGREST_SERVERSTATUS_H

#include <string>
#include <ngrest/common/Service.h>
#include <ngrest/common/Message.h>

namespace ngrest {

//! Displays deployed services
// *location: ngrest
class ServerStatus: public Service
{
public:
    // *location: services
    void getServices(MessageContext& context);

    // *location: service/{name}
    void getService(const std::string& name, MessageContext& context);

    // *location: operation/{serviceName}/{operationName}
    void getOperation(const std::string& serviceName, const std::string& operationName, MessageContext& context);
};

}

#endif
