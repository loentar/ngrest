#ifndef NGREST_EXAMPLES_FAVICON_H
#define NGREST_EXAMPLES_FAVICON_H

#include <string>
#include <ngrest/common/Service.h>
#include <ngrest/common/Message.h>

namespace ngrest {

//! Displays deployed services
// *location: favicon.ico
class Favicon: public Service
{
public:
    // *location: /
    void favicon(MessageContext& context);
};

}

#endif
