#ifndef NGREST_HTTPTRANSPORT_H
#define NGREST_HTTPTRANSPORT_H

#include "Transport.h"

namespace ngrest {

class HttpTransport: public Transport
{
public:
    // pool temporary pool for storing data
    virtual Node* parseRequest(MemPool& pool, const Request* request) override;

    // pool temporary pool for storing data
    virtual void writeResponse(MemPool& pool, const Request* request, Response* response) override;

    virtual int getRequestMethod(const Request* request) override;
};

} // namespace ngrest

#endif // NGREST_HTTPTRANSPORT_H
