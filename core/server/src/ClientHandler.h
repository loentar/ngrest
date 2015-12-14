#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <unordered_map>

#include "ClientCallback.h"

namespace ngrest {

class Exception;
class Engine;
struct Request;
struct ClientData;
class HttpResponse;
class HttpResponseCallbackHandler;

class ClientHandler: public ClientCallback
{
public:
    ClientHandler(Engine& engine);

    virtual void connected(int fd, const sockaddr* addr) override;
    virtual void disconnected(int fd) override;
    virtual void error(int fd) override;
    virtual bool readyRead(int fd) override;

private:
    void parseHttpHeader(char* buffer, Request* client);
    void processRequest(int clientFd, Request* client);
    void processResponse(int clientFd, Request* request, const HttpResponse* response);
    void processError(int clientFd, Request* request, const Exception& error);

private:
    std::unordered_map<int, ClientData*> clients;
    Engine& engine;
    friend class HttpResponseCallbackHandler;
};

}

#endif // CLIENTHANDLER_H
