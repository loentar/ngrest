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

#ifndef NGREST_STRUTILS_H
#define NGREST_STRUTILS_H

#include <string.h>

namespace ngrest {

inline const char* strnstr(const char *s, const char *find, size_t slen)
{
    char c;
    char sc;

    if ((c = *find++) != '\0') {
        size_t len = strlen(find);
        do {
            do {
                if ((sc = *s++) == '\0' || slen-- < 1)
                    return nullptr;
            } while (sc != c);
            if (len > slen)
                return nullptr;
        } while (strncmp(s, find, len) != 0);
        s--;
    }
    return s;
}


inline char* token(char*& curr) {
    char* start = curr;
    while (*curr != ' ' && *curr != '\t' && *curr != '\n' && *curr != '\r' && *curr != '\0')
        ++curr;
    *curr = '\0';
    ++curr;
    return start;
}

inline char* token(char*& curr, char delimiter) {
    char* start = curr;
    while (*curr != delimiter && *curr != '\0')
        ++curr;
    *curr = '\0';
    ++curr;
    return start;
}

inline void skipWs(char*& curr)
{
    while (*curr == ' ' || *curr == '\t' || *curr == '\n' || *curr == '\r')
        ++curr;
}

inline bool seekTo(char*& curr, char ch)
{
    for (; *curr != '\0'; ++curr)
        if (*curr == ch)
            return true;
    return false;
}

inline void trimRight(char* start, char* end)
{
    char* found = nullptr;
    while ((end > start) && (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
        found = end;
        --end;
    }

    if (found)
        *found = '\0';
}

// latin1
inline void toLowerCase(char* str)
{
    for (; *str; ++str)
        *str = tolower(*str);
}

}

#endif // NGREST_STRUTILS_H

