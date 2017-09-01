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

#ifndef NGREST_RUNTIME_H
#define NGREST_RUNTIME_H

#include <string>
#include "ngrestutilsexport.h"

namespace ngrest {

/**
 * @brief ngrest runtime. stores misc shared information
 */
class NGREST_UTILS_EXPORT Runtime
{
public:
    /**
     * @brief get path to appliation root
     * @return parent directory of the application executable
     */
    static const std::string& getApplicationRootPath();

    /**
     * @brief get path to appliation directory
     * @return directory of the application executable
     */
    static const std::string& getApplicationDirPath();

    /**
     * @brief get path to appliation executable
     * @return full path to application executable
     */
    static const std::string& getApplicationFilePath();

    /**
     * @brief get path to share directory
     * @return path to share directory
     */
    static const std::string& getSharePath();
};

} // namespace ngrest

#endif // NGREST_RUNTIME_H
