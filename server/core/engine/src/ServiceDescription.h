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

#ifndef NGREST_SERVICEDESCRIPTION_H
#define NGREST_SERVICEDESCRIPTION_H

#include <string>
#include <vector>

namespace ngrest {

/**
 * @brief operation parameter description
 */
struct ParameterDescription
{
    /**
     * @brief type of operation parameter
     */
    enum class Type
    {
        Unknown,
        Undefined,
        String,
        Number,
        Boolean,
        Array,
        Object,
        Any
    };

    std::string name;  //!< parameter name
    Type type;         //!< type of parameter
    bool nullable;     //!< can be null
};

/**
 * @brief operation description
 */
struct OperationDescription
{
    std::string name;                              //!< service operation
    std::string location;                          //!< by default = name, can be "add?a={a}&b={b}" or "get/{id}" or "put"
    int method;                                    //!< method depending on transport
    std::string methodStr;                         //!< method depending on transport in string form
    bool asynchronous;                             //!< is operation asynchronous
    std::string description;                       //!< text description of the operation
    std::string details;                           //!< text details of the operation
    std::vector<ParameterDescription> parameters;  //!< parameters
    ParameterDescription::Type result;             //!< type of result value
    bool resultNullable;                           //!< result can be null
};

/**
 * @brief service description
 */
struct ServiceDescription
{
    std::string name;                              //!< service name
    std::string location;                          //!< by default = name
    std::string description;                       //!< text description of the service
    std::string details;                           //!< text details of the service
    std::vector<OperationDescription> operations;  //!< service operations
};

} // namespace ngrest

#endif // NGREST_SERVICEDESCRIPTION_H
