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

#ifndef NGREST_FILTER_H
#define NGREST_FILTER_H

#include <string>
#include <list>

#include "ngrestengineexport.h"

namespace ngrest {

enum class Phase;
struct MessageContext;

/**
 * @brief Message Filter
 */
class NGREST_ENGINE_EXPORT Filter
{
public:
    /**
     * @brief destructor
     */
    virtual ~Filter();

    /**
     * @brief get filter name
     * @return filter name
     */
    virtual const std::string& getName() const = 0;

    /**
     * @brief get filter dependencies
     * @return list of filter names which this filter depends on
     */
    virtual const std::list<std::string>& getDependencies() const = 0;

    /**
     * @brief process message through filter
     * @param phase filter phase
     * @param context message context
     */
    virtual void filter(Phase phase, MessageContext* context) = 0;
};

}

#endif // NGREST_FILTER_H
