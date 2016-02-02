/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

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

#include "TestServiceGroup.h"

int help() {
    std::cerr << "ngrest_server [-h][-p <PORT>]" << std::endl
              << "  -p        port number to use (default: 9098)" << std::endl
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


    /*
    test with:
    async mode:
      curl -i -X GET -H "Content-Type:application/json" 'http://localhost:9098/td/sync?value=Hello_world'

    sync mode:
      curl -i -X GET -H "Content-Type:application/json" 'http://localhost:9098/td/async/Hello_world'
    */

    ngrest::TestServiceGroup group;
    deployment.deployStatic(&group);

    return server.exec();
}
