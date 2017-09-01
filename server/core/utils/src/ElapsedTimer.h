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

#ifndef NGREST_ELAPSEDTIMER_H
#define NGREST_ELAPSEDTIMER_H

#include <stdint.h>
#include "ngrestutilsexport.h"

namespace ngrest {

/**
 * @brief class to calculate elapsed time
 */
class NGREST_UTILS_EXPORT ElapsedTimer
{
public:
    inline ElapsedTimer(bool startNow = false)
    {
        if (startNow)
            start();
    }

    /**
     * @brief start measuring
     */
    inline void start()
    {
        started = getTime();
    }

    /**
     * @brief returns the number of microseconds since this ElapsedTimer was started
     */
    inline int64_t elapsed() const
    {
        return getTime() - started;
    }

    /**
     * @brief get current time in microseconds
     */
    static int64_t getTime();

private:
    int64_t started = 0;
};

} // namespace ngrest

#endif // NGREST_ELAPSEDTIMER_H
