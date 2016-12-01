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

#ifndef NGREST_LOOPER_H
#define NGREST_LOOPER_H

#include <functional>
#include "ngrestengineexport.h"

namespace ngrest {

typedef std::function<void()> Task; //!< a task to execute on event loop

/**
 * @brief Base class for event loop
 */
class NGREST_ENGINE_EXPORT Looper
{
public:
    /**
     * @brief destructor
     */
    virtual ~Looper();

    /**
     * @brief post task to event loop.
     * This function is thread-safe if ngrest is compiled with WITH_THREAD_LOCK
     * @param task
     */
    virtual void post(Task task) = 0;

    /**
     * @brief get instance of main looper
     * @return main event looper
     */
    inline static Looper* getMainLooper()
    {
        return mainLooper;
    }

    /**
     * @brief set set instance of main looper
     * @param looper main looper
     */
    inline static void setMainLooper(Looper* looper)
    {
        mainLooper = looper;
    }

private:
    static Looper* mainLooper;
};

} // namespace ngrest

#endif // NGREST_LOOPER_H
