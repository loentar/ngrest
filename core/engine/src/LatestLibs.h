/*
 *  Copyright 2017 NAM system, a.s.
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

#ifndef NGREST_LATESTLIBS_H
#define NGREST_LATESTLIBS_H

#include <string.h>
#include <unordered_map>

#include <ngrest/utils/File.h>
#include "LatestLibs.h"

namespace ngrest {

/**
 * @brief List of libraries with one item for all numbers in form of "filename.so.number". List is filtered by extension NGREST_LIBRARY_EXT, which must be either suffix or just before version number.
 */
class LatestLibs {
public:
    /**
     * @brief Item in map containg original filename and version from suffix.
     */
    struct Version {
        std::string filename;
        int version;
    };

    /**
     * @brief Map of latest libraries. Key is filename with version number removed, value has full filename.
     */
    std::unordered_map<std::string, Version> map;

    /**
     * @brief Constructor takes full list of files and put unique filename with highest version into map. During input list iteration versions are compared with filenames read so far.
     * @param allLibs list of files in libraries directory
     */
    LatestLibs(const StringList& allLibs) {
        const size_t extlen = strlen(NGREST_LIBRARY_EXT);

        for (const std::string& lib : allLibs) {
            size_t len = lib.length();
            if (len > extlen) {
                if (lib.compare(len - extlen, extlen, NGREST_LIBRARY_EXT) == 0) {
                    map[lib] = { lib, 0 };
                }
                else {
                    size_t pos = lib.find_last_of('.');
                    if (pos != std::string::npos && pos > extlen) {
                        const char* lastdot = lib.c_str() + pos;
                        if (!strncmp(lastdot - extlen, NGREST_LIBRARY_EXT, extlen)) {
                            char* endptr;
                            int version = strtol(lastdot + 1, &endptr, 10);
                            if (!*endptr) {
                                Version& v = map[lib.substr(0, pos)];
                                if (v.version <= version) {
                                    v.version = version;
                                    v.filename = lib;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
};

} // namespace ngrest

#endif // NGREST_LATESTLIBS_H
