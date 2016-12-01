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

#ifndef NGREST_FILTERDEPLOYMENT_H
#define NGREST_FILTERDEPLOYMENT_H

#include <string>

#include "ngrestengineexport.h"

namespace ngrest {

class FilterDispatcher;
class FilterGroup;

/**
 * @brief Filterdeployment management class
 */
class NGREST_ENGINE_EXPORT FilterDeployment
{
public:
    /**
     * @brief constructor
     * @param dispatcher filter dispatcher to deploy filters to
     */
    FilterDeployment(FilterDispatcher& dispatcher);

    /**
     * @brief destructor
     */
    ~FilterDeployment();

    /**
     * @brief deploy all filter libraries from path given
     * @param filtersPath path to directory where filter libraries are placed
     */
    void deployAll(const std::string& filtersPath);

    /**
     * @brief deploy one filter from library
     * @param filterPath path to filter library
     */
    void deploy(const std::string& filterPath);

    /**
     * @brief undeploy the filter by library path
     * @param filterPath path to filter library to undeploy
     */
    void undeploy(const std::string& filterPath);

    /**
     * @brief undeploy all the filters
     */
    void undeployAll();

    /**
     * @brief deploy filters from filter group
     *   used to deploy compiled-in filters
     * @param filterGroup filter group to deploy
     */
    void deployStatic(FilterGroup* filterGroup);

    /**
     * @brief undeploy filter group
     * @param filterGroup filter group to undeploy
     */
    void undeployStatic(FilterGroup* filterGroup);

private:
    FilterDeployment(const FilterDeployment&);
    FilterDeployment& operator=(const FilterDeployment&);

private:
    class Impl;
    Impl* impl;
};

} // namespace ngrest

#endif // NGREST_FILTERDEPLOYMENT_H
