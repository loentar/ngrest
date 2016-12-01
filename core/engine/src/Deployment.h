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

#ifndef NGREST_DEPLOYMENT_H
#define NGREST_DEPLOYMENT_H

#include <string>

#include "ngrestengineexport.h"

namespace ngrest {

class ServiceDispatcher;
class ServiceGroup;

/**
 * @brief deployment management class
 */
class NGREST_ENGINE_EXPORT Deployment
{
public:
    /**
     * @brief constructor
     * @param dispatcher service dispatcher to deploy services to
     */
    Deployment(ServiceDispatcher& dispatcher);

    /**
     * @brief destructor
     */
    ~Deployment();

    /**
     * @brief deploy all service libraries from path given
     * @param servicesPath path to directory where service libraries are placed
     */
    void deployAll(const std::string& servicesPath);

    /**
     * @brief deploy one service from library
     * @param servicePath path to service library
     */
    void deploy(const std::string& servicePath);

    /**
     * @brief undeploy the service by library path
     * @param servicePath path to service library to undeploy
     */
    void undeploy(const std::string& servicePath);

    /**
     * @brief undeploy all the services
     */
    void undeployAll();

    /**
     * @brief deploy services from service group
     *   used to deploy compiled-in services
     * @param serviceGroup service group to deploy
     */
    void deployStatic(ServiceGroup* serviceGroup);

    /**
     * @brief undeploy service group
     * @param serviceGroup service group to undeploy
     */
    void undeployStatic(ServiceGroup* serviceGroup);

private:
    Deployment(const Deployment&);
    Deployment& operator=(const Deployment&);

private:
    class Impl;
    Impl* impl;
};

} // namespace ngrest

#endif // NGREST_DEPLOYMENT_H
