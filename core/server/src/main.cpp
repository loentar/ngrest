#include <signal.h>
#include <iostream>

#include <ngrest/utils/Log.h>

#include <ngrest/engine/Engine.h>

#include "servercommon.h"
#include "Server.h"
#include "ClientHandler.h"

int help() {
    std::cerr << "ngrest_server [-h][-p <PORT>]" << std::endl
              << "  -p        port number to use (default: 9099)" << std::endl
              << "  -h        display this help" << std::endl << std::endl;
    return 1;
}

class SignalHandler
{
public:
    SignalHandler(ngrest::Server* server)
    {
        this->server = server;
        ::signal(SIGINT, onSignal);
        ::signal(SIGTERM, onSignal);
    }

    static void onSignal(int /*sig*/)
    {
        ngrest::LogInfo() << "Stopping server";
        server->quit();
    }

private:
    static ngrest::Server* server;
};

ngrest::Server* SignalHandler::server = nullptr;



int main(int argc, char* argv[])
{
    ngrest::StringMap args;

    for (int i = 1; i < argc; i += 2) {
        if (argv[i][0] != '-' || argv[i][1] == '\0' || (i + 1) == argc) {
            return help();
        }

        args[argv[i] + 1] = argv[i + 1];
    }

    ngrest::Server server;
    ngrest::Engine engine;
    ngrest::ClientHandler clientHandler(engine);

    server.setClientCallback(&clientHandler);

    if (!server.create(args))
        return 1;

    SignalHandler handler(&server);

    return server.exec();
}
