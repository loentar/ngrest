#include <ngrest/utils/Exception.h>
#include <ngrest/utils/stringutils.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/engine/Transport.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/ServiceDispatcher.h>
#include <ngrest/engine/ServiceWrapper.h>
#include <ngrest/engine/ServiceDescription.h>

#include "ServerStatus.h"

namespace ngrest {

static const char* css =
"html {\
font-family:sans-serif;\
font-size: 14px;\
}\
a {\
color: #18d;\
text-decoration: none;\
}\
a:hover, a:active {\
color: #07c;\
text-decoration: underline;\
}";

std::string getServiceLocation(const ServiceDescription* serviceDescr)
{
    if (serviceDescr->location.empty()) {
        std::string serviceLocation = serviceDescr->name;
        stringReplace(serviceLocation, ".", "/", true);
        return serviceLocation;
    } else {
        return serviceDescr->location;
    }
}

void ServerStatus::getServices(MessageContext& context)
{
    NGREST_ASSERT(context.transport->getType() == Transport::Type::Http,
                  "Only HTTP transport supported to get status");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    Header* headerContentType = context.pool.alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    MemPool& pool = context.response->poolBody;

    pool.putCString("<html><head><style>");
    pool.putCString(css);
    pool.putCString("</style></head><body>"
                    "<h1>ngrest<h1>"
                    "<h2>Deployed services:</h2>");
    const std::vector<ServiceWrapper*>& services = context.engine->getDispatcher().getServices();
    for (auto itSvc = services.begin(), endService = services.end(); itSvc != endService; ++itSvc) {
        const ServiceWrapper* service = *itSvc;
        const ServiceDescription* descr = service->getDescription();
        pool.putCString("<p><h3>");
        pool.putCString(descr->name.c_str());
        pool.putCString("</h3><ul>");
        const std::string& serviceLocation = getServiceLocation(descr);
        for (auto itOp = descr->operations.begin(), endOp = descr->operations.end(); itOp != endOp; ++itOp) {
            const OperationDescription* opDescr = &*itOp;
            pool.putCString(("<li><a href=\"/" + serviceLocation + "/" + opDescr->location + "\">").c_str());
            pool.putCString(opDescr->name.c_str());
            pool.putCString("</a></li>");
        }
        pool.putCString("</ul></p>");
    }

    pool.putCString("</body></html>");

    context.callback->success(&context);
}

}
