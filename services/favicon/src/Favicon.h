#ifndef NGREST_FAVICON_H
#define NGREST_FAVICON_H

#include <ngrest/common/Service.h>
#include <ngrest/common/Message.h>

namespace ngrest {

//! Simple service to display favicon in browser
// *location: favicon.ico
class Favicon: public Service
{
public:
    // *location: /
    void favicon(MessageContext& context);
};

}

#endif
