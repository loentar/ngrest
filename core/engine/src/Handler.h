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

#ifndef NGREST_HANDLER_H
#define NGREST_HANDLER_H

#include "Looper.h"
#include "ngrestengineexport.h"

namespace ngrest {

/**
 * @brief Handler class to post task to event loop
 */
class NGREST_ENGINE_EXPORT Handler
{
public:
    /**
     * @brief post task to event loop (main thread).
     * This function is thread-safe if ngrest is compiled with WITH_THREAD_LOCK
     * @param task task to post
     */
    static void post(Task task);
};

} // namespace ngrest

#endif // NGREST_HANDLER_H
