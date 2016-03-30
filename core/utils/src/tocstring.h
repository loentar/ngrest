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

#ifndef NGREST_UTILS_TOCSTRING_H
#define NGREST_UTILS_TOCSTRING_H

#include <stdio.h>

#ifdef _MSC_VER
#pragma warning (disable: 4996)
#define ngrest_snprintf _snprintf
#else
#define ngrest_snprintf snprintf
#endif

#if defined WIN32 && defined __GNUC__
#pragma GCC diagnostic push
// suppress buggy warnings
#pragma GCC diagnostic ignored "-Wformat="
#pragma GCC diagnostic ignored "-Wformat-extra-args"
#endif

namespace ngrest {

typedef char byte;
typedef unsigned char unsignedByte;

#ifndef NGREST_NUM_TO_STR_BUFF_SIZE
#define NGREST_NUM_TO_STR_BUFF_SIZE 32
#endif


inline bool toCString(bool value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%s", value ? "true" : "false") < bufferSize;
}


inline bool toCString(byte value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%d", static_cast<signed char>(value)) < bufferSize;
}

inline bool toCString(int value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%d", value) < bufferSize;
}

inline bool toCString(short value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%d", value) < bufferSize;
}

inline bool toCString(long value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%ld", value) < bufferSize;
}

inline bool toCString(long long value, char* buffer, int bufferSize)
{
#ifdef WIN32
    return ngrest_snprintf(buffer, bufferSize, "%I64d", value) < bufferSize;
#else
    return ngrest_snprintf(buffer, bufferSize, "%lld", value) < bufferSize;
#endif
}


inline bool toCString(unsignedByte value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%d", value) < bufferSize;
}

inline bool toCString(unsigned int value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%u", value) < bufferSize;
}

inline bool toCString(unsigned short value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%u", value) < bufferSize;
}

inline bool toCString(unsigned long value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%lu", value) < bufferSize;
}

inline bool toCString(unsigned long long value, char* buffer, int bufferSize)
{
#ifdef WIN32
    return ngrest_snprintf(buffer, bufferSize, "%I64u", value) < bufferSize;
#else
    return ngrest_snprintf(buffer, bufferSize, "%llu", value) < bufferSize;
#endif
}


inline bool toCString(float value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%f", value) < bufferSize;
}

inline bool toCString(double value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "%f", value) < bufferSize;
}

inline bool toCString(long double value, char* buffer, int bufferSize)
{
#ifdef WIN32
    return ngrest_snprintf(buffer, bufferSize, "%Le", value) < bufferSize;
#else
    return ngrest_snprintf(buffer, bufferSize, "%Lf", value) < bufferSize;
#endif
}

inline bool toCString(float value, char* buffer, int bufferSize, const char* precision)
{
    char format[NGREST_NUM_TO_STR_BUFF_SIZE];
    if (ngrest_snprintf(format, bufferSize, "%%%sf", precision) >= bufferSize)
        return false;

    return ngrest_snprintf(buffer, bufferSize, format, value) < bufferSize;
}

inline bool toCString(double value, char* buffer, int bufferSize, const char* precision)
{
    char format[NGREST_NUM_TO_STR_BUFF_SIZE];
    if (ngrest_snprintf(format, bufferSize, "%%%sf", precision) >= bufferSize)
        return false;

    return ngrest_snprintf(buffer, bufferSize, format, value) < bufferSize;
}

inline bool toCString(long double value, char* buffer, int bufferSize, const char* precision)
{
    char format[NGREST_NUM_TO_STR_BUFF_SIZE];
#ifdef WIN32
    if (ngrest_snprintf(format, bufferSize, "%%%sLe", precision) >= bufferSize)
#else
    if (ngrest_snprintf(format, bufferSize, "%%%sLf", precision) >= bufferSize)
#endif
        return false;

    return ngrest_snprintf(buffer, bufferSize, format, value) < bufferSize;
}


inline bool toHexCString(unsignedByte value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "0x%x", value) < bufferSize;
}

inline bool toHexCString(unsigned int value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "0x%x", value) < bufferSize;
}

inline bool toHexCString(unsigned short value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "0x%x", value) < bufferSize;
}

inline bool toHexCString(unsigned long value, char* buffer, int bufferSize)
{
    return ngrest_snprintf(buffer, bufferSize, "0x%lx", value) < bufferSize;
}

inline bool toHexCString(unsigned long long value, char* buffer, int bufferSize)
{
#ifdef WIN32
    return ngrest_snprintf(buffer, bufferSize, "0x%I64x", value) < bufferSize;
#else
    return ngrest_snprintf(buffer, bufferSize, "0x%llx", value) < bufferSize;
#endif
}

inline bool toHexCString(const void* ptr, char* buffer, int bufferSize)
{
#if defined __LP64__ || defined _M_X64 || defined __x86_64
#ifdef WIN32
    return ngrest_snprintf(buffer, bufferSize,
                           "0x%016I64x", reinterpret_cast<const unsigned long long>(ptr)) < bufferSize;
#else
    return ngrest_snprintf(buffer, bufferSize,
                           "0x%016llx", reinterpret_cast<const unsigned long long>(ptr)) < bufferSize;
#endif
#else
    return ngrest_snprintf(buffer, bufferSize,
                           "0x%08x", reinterpret_cast<const unsigned int>(ptr)) < bufferSize;
#endif
}

}

#if defined WIN32 && defined __GNUC__
#pragma GCC diagnostic pop
#endif

#endif // NGREST_UTILS_TOCSTRING_H
