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
#include <ngrest/utils/ElapsedTimer.h>
#include <ngrest/utils/Runtime.h>
#include <ngrest/utils/File.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/ServiceDispatcher.h>
#include <ngrest/engine/FilterDispatcher.h>
#include <ngrest/engine/FilterDeployment.h>
#include <ngrest/engine/Deployment.h>
#include <ngrest/engine/HttpTransport.h>
#include <ngrest/engine/Looper.h>

#include "servercommon.h"
#include "Server.h"
#include "ClientHandler.h"

#if defined WIN32 || defined __APPLE__
typedef void(__cdecl *sighandler_t)(int);
#endif

int help() {
    std::cerr << "ngrest_server [-h][-p <PORT>]" << std::endl
              << "  -s        set extra path to locate services" << std::endl
              << "  -p        port number to use (default: 9098)" << std::endl
              << "  -l        listen to specific ip (default: all)" << std::endl
              << "  -h        display this help" << std::endl << std::endl;
    return 1;
}


int main(int argc, char* argv[])
{
    ngrest::ElapsedTimer timer(true);
    ngrest::StringMap args;

    for (int i = 1; i < argc; i += 2) {
        if (argv[i][0] != '-' || argv[i][1] == '\0' || (i + 1) == argc) {
            return help();
        }

        args[argv[i] + 1] = argv[i + 1];
    }

    static ngrest::Server server;
    ngrest::ServiceDispatcher serviceDispatcher;
    ngrest::Deployment deployment(serviceDispatcher);
    ngrest::FilterDispatcher filterDispatcher;
    ngrest::FilterDeployment filterDeployment(filterDispatcher);
    ngrest::HttpTransport transport;
    ngrest::Engine engine(serviceDispatcher);
    ngrest::ClientHandler clientHandler(engine, transport);

    engine.setFilterDispatcher(&filterDispatcher);
    server.setClientCallback(&clientHandler);
    ngrest::Looper::setMainLooper(&server);

    if (!server.create(args))
        return 1;

    sighandler_t signalHandler = [] (int) {
        ngrest::LogInfo() << "Stopping server";
        server.quit();
    };

    ::signal(SIGINT, signalHandler);
    ::signal(SIGTERM, signalHandler);
#ifndef WIN32
    ::signal(SIGPIPE, SIG_IGN);
#endif

    const std::string& filtersPath = ngrest::Runtime::getSharePath()
        + NGREST_PATH_SEPARATOR "filters" NGREST_PATH_SEPARATOR;
    filterDeployment.deployAll(filtersPath, nullptr);

    const std::string& servicesPath = ngrest::Runtime::getSharePath()
        + NGREST_PATH_SEPARATOR "services" NGREST_PATH_SEPARATOR;
    deployment.deployAll(servicesPath, nullptr);

    auto itPath = args.find("s");
    if (itPath != args.end())
        deployment.deployAll(itPath->second + NGREST_PATH_SEPARATOR, nullptr);

    ngrest::LogInfo() << "Server startup time: " << (timer.elapsed() / 1000.) << "ms";

    return server.exec();
}
