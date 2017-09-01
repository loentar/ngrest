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

#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include "Exception.h"
#include "Error.h"
#include "DynamicLibrary.h"

namespace ngrest {

DynamicLibrary::DynamicLibrary()
{
}

DynamicLibrary::~DynamicLibrary()
{
}

void DynamicLibrary::load(const std::string& libName, bool raw /*= false*/)
{
    NGREST_ASSERT(!handle, "Library is already loaded");

    if (raw) {
#ifdef WIN32
        handle = LoadLibraryA(libName.c_str());
#else
        handle = dlopen(libName.c_str(), RTLD_NOW);
#endif
    } else {
#ifdef WIN32
        handle = LoadLibraryA((libName + NGREST_LIBRARY_EXT).c_str());
#else
        std::string::size_type pos = libName.find_last_of('/');
        if (pos == std::string::npos) {
            handle = dlopen((NGREST_LIBRARY_PREFIX + libName + NGREST_LIBRARY_EXT).c_str(), RTLD_LAZY);
        } else {
            handle = dlopen((libName.substr(pos) + NGREST_LIBRARY_PREFIX +
                             libName.substr(pos + 1) + NGREST_LIBRARY_EXT).c_str(), RTLD_LAZY);
        }
#endif
    }

    NGREST_ASSERT(handle, "Failed to load library [" + libName + "]: " + Error::getLastLibraryError());
    this->libName = libName;
    this->raw = raw;
}

const std::string& DynamicLibrary::name() const
{
    NGREST_ASSERT(handle, "Library is not loaded");
    return libName;
}

void* DynamicLibrary::getSymbol(const std::string& symbol) const
{
    NGREST_ASSERT(handle, "Library is not loaded");

#ifdef WIN32
    void* result = reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle), symbol.c_str()));
#else
    void* result = dlsym(handle, symbol.c_str());
#endif

    NGREST_ASSERT(result, "Failed to get symbol [" + symbol + "]: " + Error::getLastLibraryError());
    return result;
}

void DynamicLibrary::unload()
{
    NGREST_ASSERT(handle, "Library is not loaded");
#ifdef WIN32
    FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
    handle = nullptr;
}

void DynamicLibrary::reload()
{
    unload();
    load(libName, raw);
}

}
