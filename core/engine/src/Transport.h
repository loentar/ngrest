#ifndef NGREST_TRANSPORT_H
#define NGREST_TRANSPORT_H

namespace ngrest {

class MemPool;
class Request;
class Response;
class Node;

//enum class TransportType
//{
//    Unknown,
//    Http,
//    User
//};

class Transport
{
public:
//    Transport(TransportType transportType);
    virtual ~Transport();

//    inline TransportType type() const
//    {
//        return transportType;
//    }

    virtual Node* parseRequest(MemPool& pool, const Request* request) = 0;
    virtual void writeResponse(MemPool& pool, const Request* request, Response* response) = 0;

    virtual int getRequestMethod(const Request* request) = 0;

//private:
//    TransportType transportType;
};

} // namespace ngrest

#endif // NGREST_TRANSPORT_H
