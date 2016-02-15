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

#ifndef NGREST_UTILS_FROMCSTRING_H
#define NGREST_UTILS_FROMCSTRING_H

#include <string.h>
#include <stdlib.h>

#ifdef _MSC_VER
#define ngrest_strtoll _strtoi64
#define ngrest_strtoull _strtoui64
#define ngrest_strtof(str, end) static_cast<float>(strtod(str, end))
#else
#define ngrest_strtoll strtoll
#define ngrest_strtoull strtoull
#define ngrest_strtof strtof
#endif

#if defined _MSC_VER || defined __ANDROID_API__
#define ngrest_strtold(str, end) static_cast<long double>(strtod(str, end))
#else
#define ngrest_strtold strtold
#endif

namespace ngrest {

typedef char byte;
typedef unsigned char unsignedByte;

inline bool fromCString(const char* string, bool& value)
{
    if (!strcmp(string, "true") || !strcmp(string, "1")) {
        value = true;
        return true;
    }

    if (!strcmp(string, "false") || !strcmp(string, "0")) {
        value = false;
        return true;
    }

    return false;
}


inline bool fromCString(const char* string, byte& value) {
    char* end = nullptr;
    value = static_cast<byte>(strtol(string, &end, 10));
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, int& value)
{
    char* end = nullptr;
    value = static_cast<int>(strtol(string, &end, 10));
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, short& value)
{
    char* end = nullptr;
    value = static_cast<short>(strtol(string, &end, 10));
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, long& value)
{
    char* end = nullptr;
    value = strtol(string, &end, 10);
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, long long& value)
{
    char* end = nullptr;
    value = ngrest_strtoll(string, &end, 10);
    return !end || (*end == '\0' && end != string);
}


inline bool fromCString(const char* string, unsignedByte& value)
{
    char* end = nullptr;
    value = static_cast<unsignedByte>(strtoul(string, &end, 10));
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, unsigned int& value)
{
    char* end = nullptr;
    value = static_cast<unsigned int>(strtoul(string, &end, 10));
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, unsigned short& value)
{
    char* end = nullptr;
    value = static_cast<unsigned short>(strtoul(string, &end, 10));
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, unsigned long& value)
{
    char* end = nullptr;
    value = strtoul(string, &end, 10);
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, unsigned long long& value)
{
    char* end = nullptr;
    value = ngrest_strtoull(string, &end, 10);
    return !end || (*end == '\0' && end != string);
}


inline bool fromCString(const char* string, float& value)
{
    char* end = nullptr;
    value = ngrest_strtof(string, &end);
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, double& value)
{
    char* end = nullptr;
    value = strtod(string, &end);
    return !end || (*end == '\0' && end != string);
}

inline bool fromCString(const char* string, long double& value)
{
    char* end = nullptr;
    value = ngrest_strtold(string, &end);
    return !end || (*end == '\0' && end != string);
}

template <typename Type>
inline Type fromCStringDefault(const char* string, Type defaultValue)
{
    Type result = 0;
    return (string != nullptr && fromCString(string, result)) ? result : defaultValue;
}


inline bool fromHexCString(const char* string, unsignedByte& value)
{
    char* end = nullptr;
    value = static_cast<unsignedByte>(strtoul(string, &end, 16));
    return !end || (*end == '\0' && end != string);
}

inline bool fromHexCString(const char* string, unsigned int& value)
{
    char* end = nullptr;
    value = static_cast<unsigned int>(strtoul(string, &end, 16));
    return !end || (*end == '\0' && end != string);
}

inline bool fromHexCString(const char* string, unsigned short& value)
{
    char* end = nullptr;
    value = static_cast<unsigned short>(strtoul(string, &end, 16));
    return !end || (*end == '\0' && end != string);
}

inline bool fromHexCString(const char* string, unsigned long& value)
{
    char* end = nullptr;
    value = strtoul(string, &end, 16);
    return !end || (*end == '\0' && end != string);
}

inline bool fromHexCString(const char* string, unsigned long long& value)
{
    char* end = nullptr;
    value = ngrest_strtoull(string, &end, 16);
    return !end || (*end == '\0' && end != string);
}

template <typename Type>
inline Type FromHexCStringDefault(const char* string, Type defaultValue)
{
    Type result = 0;
    return (string != nullptr && fromHexCString(string, result)) ? result : defaultValue;
}

}

#endif // NGREST_UTILS_FROMCSTRING_H
