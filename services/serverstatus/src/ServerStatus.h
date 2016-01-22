#ifndef NGREST_EXAMPLES_SERVERSTATUS_H
#define NGREST_EXAMPLES_SERVERSTATUS_H

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
};

}

#endif
