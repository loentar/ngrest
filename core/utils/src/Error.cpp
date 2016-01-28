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

#ifdef WIN32
#include <windows.h>
#else
#include <errno.h>
#include <string.h>
#include <dlfcn.h>
#endif
#include "Error.h"

namespace ngrest {

std::string Error::getError(long errorNo)
{
#ifdef WIN32
    LPVOID buff = nullptr;
    DWORD count = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorNo, 0,
                                 reinterpret_cast<LPSTR>(&buff), 0, nullptr);

    if (!count || !buff) {
        return "<Unknown error>";
    } else {
        std::string error(reinterpret_cast<LPCSTR>(buff), static_cast<std::string::size_type>(count));
        LocalFree(buff);
        return error;
    }
#else
    return strerror(errorNo);
#endif
}

std::string Error::getLastError()
{
#ifdef WIN32
    return getError(GetLastError());
#else
    return getError(errno);
#endif
}

std::string Error::getLastLibraryError()
{
#ifdef WIN32
    return getError(GetLastError());
#else
    return dlerror();
#endif
}


}
