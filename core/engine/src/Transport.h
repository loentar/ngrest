#ifndef NGREST_TRANSPORT_H
#define NGREST_TRANSPORT_H

namespace ngrest {

class MemPool;
class Request;
class Response;
class Node;

//! abstract transport to translate request to OM and OM to response
class Transport
{
public:
    //! transport type
    enum class Type
    {
        Unknown,
        Http,
        User
    };

public:
    Transport(Type type);
    virtual ~Transport();

    /**
     * @brief get transport type
     * @return transport type
     */
    inline Type getType() const
    {
        return type;
    }

    /**
     * @brief parse incoming transport request and translate it to OM request
     * @param pool pool to store temporary and OM data
     * @param request incoming request
     * @return OM translated from request
     */
    virtual Node* parseRequest(MemPool& pool, const Request* request) = 0;

    /**
     * @brief translate OM response to transport response
     * @param pool pool to store temporary data
     * @param request source request
     * @param response response translated from OM
     */
    virtual void writeResponse(MemPool& pool, const Request* request, Response* response) = 0;

    /**
     * @brief get request method
     * @param request request
     * @return request method
     */
    virtual int getRequestMethod(const Request* request) = 0;

private:
    Type type;
};

} // namespace ngrest

#endif // NGREST_TRANSPORT_H
