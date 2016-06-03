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

#ifndef NGREST_UTILS_ERROR_H
#define NGREST_UTILS_ERROR_H

#include <string>
#include "ngrestutilsexport.h"

namespace ngrest {

/**
 * @brief get os error
 */
class NGREST_UTILS_EXPORT Error
{
public:
    /**
     * @brief get error string by error no
     * @param errorNo - error no
     * @return error string
     */
    static std::string getError(long errorNo);

    /**
     * @brief get last error string
     * @return last error string
     */
    static std::string getLastError();

    /**
     * @brief get last library error string
     * @return last library error string
     */
    static std::string getLastLibraryError();
};

}

#endif // NGREST_UTILS_ERROR_H
