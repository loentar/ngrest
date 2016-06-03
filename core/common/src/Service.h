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

#ifndef NGREST_SERVICE_H
#define NGREST_SERVICE_H

#include <string>
#include "ngrestcommonexport.h"

namespace ngrest {

/**
 * @brief abstract parent for all ngrest services
 */
class NGREST_COMMON_EXPORT Service
{
public:
    /**
     * @brief constructor
     */
    Service();

    /**
     * @brief destructor
     */
    virtual ~Service();
};

} // namespace ngrest

#endif // NGREST_SERVICE_H
