#ifndef NGREST_CLIENTCALLBACK_H
#define NGREST_CLIENTCALLBACK_H

struct sockaddr;

namespace ngrest {

class ClientCallback {
public:
    virtual ~ClientCallback()
    {
    }

    virtual void connected(int fd, const sockaddr* addr) = 0;
    virtual void disconnected(int fd) = 0;
    virtual void error(int fd) = 0;
    virtual bool readyRead(int fd) = 0;
};

}

#endif // NGREST_CLIENTCALLBACK_H

