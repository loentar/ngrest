#include <unordered_map>

#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/HttpException.h>
#include <ngrest/engine/Transport.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/ServiceDispatcher.h>
#include <ngrest/engine/ServiceWrapper.h>
#include <ngrest/engine/ServiceDescription.h>

#include "Favicon.h"
#include "favicon_ico.h"

namespace ngrest {

void Favicon::favicon(MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    Header* headerContentType = context.pool.alloc<Header>("Content-Type", "image/x-icon");
    response->headers = headerContentType;

    context.response->poolBody.putData(reinterpret_cast<const char*>(favicon_ico), favicon_ico_len);
}

}
