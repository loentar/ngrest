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
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#else
#include <windows.h>
#endif
#include <limits.h>
#ifdef __FreeBSD__
#include <sys/types.h>
#include <sys/sysctl.h>
#elif defined __APPLE__
#include <mach-o/dyld.h>
#endif

#include "Exception.h"
#include "File.h"
#include "Runtime.h"

namespace ngrest {

const std::string& Runtime::getApplicationRootPath()
{
    static std::string result;
    if (result.empty()) {
        result = getApplicationDirPath();
        std::string::size_type pos = result.find_last_of(NGREST_PATH_SEPARATOR);
        if (pos != std::string::npos)
            result.erase(pos);
    }

    return result;
}

const std::string& Runtime::getApplicationDirPath()
{
    static std::string result;
    if (result.empty()) {
        result = getApplicationFilePath();
        std::string::size_type pos = result.find_last_of(NGREST_PATH_SEPARATOR);
        if (pos != std::string::npos)
            result.erase(pos);
    }

    return result;
}

const std::string& Runtime::getApplicationFilePath()
{
    static std::string result;
    if (!result.empty())
        return result;

#ifdef WIN32
    char path[MAX_PATH];
    DWORD pathSize = GetModuleFileNameA(NULL, path, MAX_PATH);
#else
    char path[PATH_MAX];
#ifdef __FreeBSD__
    size_t pathSize = PATH_MAX;
    int anMib[4];
    anMib[0] = CTL_KERN;
    anMib[1] = KERN_PROC;
    anMib[2] = KERN_PROC_PATHNAME;
    anMib[3] = -1;
    sysctl(anMib, 4, path, &pathSize, NULL, 0);
#else
#ifdef __APPLE__
    uint32_t pathSize = PATH_MAX;
    _NSGetExecutablePath(path, &pathSize);
    pathSize = strlen(path);
#else
    int pathSize = readlink("/proc/self/exe", path, PATH_MAX);
#endif
#endif
#endif

    NGREST_ASSERT(pathSize > 0, "Failed to get application file path");

    result.assign(path, pathSize);

    return result;
}

const std::string& Runtime::getSharePath()
{
    static std::string result;
    if (result.empty()) {
        result = getApplicationRootPath() + NGREST_PATH_SEPARATOR "share"
                NGREST_PATH_SEPARATOR "ngrest";
    }

    return result;
}

} // namespace ngrest
