#include <signal.h>
#include <iostream>

#include <ngrest/utils/Log.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/ServiceDispatcher.h>
#include <ngrest/engine/Deployment.h>
#include <ngrest/engine/HttpTransport.h>

#include "servercommon.h"
#include "Server.h"
#include "ClientHandler.h"

int help() {
    std::cerr << "ngrest_server [-h][-p <PORT>]" << std::endl
              << "  -p        port number to use (default: 9099)" << std::endl
              << "  -h        display this help" << std::endl << std::endl;
    return 1;
}


int main(int argc, char* argv[])
{
    ngrest::StringMap args;

    for (int i = 1; i < argc; i += 2) {
        if (argv[i][0] != '-' || argv[i][1] == '\0' || (i + 1) == argc) {
            return help();
        }

        args[argv[i] + 1] = argv[i + 1];
    }

    static ngrest::Server server;
    ngrest::ServiceDispatcher dispatcher;
    ngrest::Deployment deployment(dispatcher);
    ngrest::HttpTransport transport;
    ngrest::Engine engine(dispatcher);
    ngrest::ClientHandler clientHandler(engine, transport);

    server.setClientCallback(&clientHandler);

    if (!server.create(args))
        return 1;

    sighandler_t signalHandler = [] (int) {
        ngrest::LogInfo() << "Stopping server";
        server.quit();
    };

    ::signal(SIGINT, signalHandler);
    ::signal(SIGTERM, signalHandler);

    deployment.deployAll();

    // TODO: implement file system watcher to re-load service dynamically


    return server.exec();
}
