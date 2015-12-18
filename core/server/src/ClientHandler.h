#ifndef CLIENTHANDLER_H
#define CLIENTHANDLER_H

#include <unordered_map>

#include "ClientCallback.h"

namespace ngrest {

class Exception;
class Engine;
class Transport;
struct MessageData;
struct ClientInfo;

class ClientHandler: public ClientCallback
{
public:
    ClientHandler(Engine& engine, Transport& transport);

    virtual void connected(int fd, const sockaddr* addr) override;
    virtual void disconnected(int fd) override;
    virtual void error(int fd) override;
    virtual bool readyRead(int fd) override;

    void parseHttpHeader(char* buffer, MessageData* messageData);
    void processRequest(int clientFd, MessageData* messageData);
    void processResponse(int clientFd, MessageData* messageData);
    void processError(int clientFd, MessageData* messageData, const Exception& error);

private:
    std::unordered_map<int, ClientInfo*> clients;
    Engine& engine;
    Transport& transport;
};

}

#endif // CLIENTHANDLER_H
