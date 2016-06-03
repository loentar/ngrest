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

#ifndef NGREST_UTILS_DYNAMICLIBRARY_H
#define NGREST_UTILS_DYNAMICLIBRARY_H

#include <string>
#include "ngrestutilsexport.h"

#ifdef WIN32
#define NGREST_LIBRARY_PREFIX ""
#define NGREST_LIBRARY_EXT ".dll"
#else
#ifdef __APPLE__
#define NGREST_LIBRARY_PREFIX "lib"
#define NGREST_LIBRARY_EXT ".dylib"
#else
#define NGREST_LIBRARY_PREFIX "lib"
#define NGREST_LIBRARY_EXT ".so"
#endif
#endif

namespace ngrest {

/**
 * @brief dynamic library
 */
class NGREST_UTILS_EXPORT DynamicLibrary
{
public:
    /**
     * @brief constructor
     */
    DynamicLibrary();

    /**
     * @brief destructor
     */
    virtual ~DynamicLibrary();

    /**
     * @brief load dynamic library
     * @param libName library name
     * @param raw if true libName contains full path to library
     */
    void load(const std::string& libName, bool raw = false);

    /**
     * @brief get library name
     * @return library name
     */
    const std::string& name() const;

    /**
     * @brief get pointer to library symbol
     * @param symbol symbol name
     * @return pointer to symbol
     */
    void* getSymbol(const std::string& symbol) const;

    /**
     * @brief unload library
     */
    void unload();

    /**
     * @brief unload and load library again
     */
    void reload();

private:
    void* handle = nullptr; //!< library handle
    std::string libName;    //!< library name
    bool raw = false;       //!< libName is full
};

} // namespace ngrest

#endif // NGREST_UTILS_DYNAMICLIBRARY_H
