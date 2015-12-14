#ifndef NGREST_SERVER_H
#define NGREST_SERVER_H

#include "servercommon.h"

struct epoll_event;

namespace ngrest {

class ClientCallback;

class Server
{
public:
    Server();
    ~Server();

    bool create(const StringMap& args);

    void setClientCallback(ClientCallback* callback);

    int exec();

    void quit();

private:
    int createServerSocket(const std::string& port);
    bool setupNonblock(int fd);
    bool handleIncomingConnection();
    bool handleRequest(int index);

private:
    bool isStopping = false;
    int fdServer = 0;
    int fdEpoll = 0;
    epoll_event* event = nullptr;
    epoll_event* events = nullptr;
    ClientCallback* callback = nullptr;
};

}

#endif // NGREST_SERVER_H
