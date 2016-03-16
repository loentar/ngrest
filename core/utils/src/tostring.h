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

#ifndef NGREST_UTILS_TOSTRING_H
#define NGREST_UTILS_TOSTRING_H

#include <string>
#include "tocstring.h"

namespace ngrest {

template<typename Type>
std::string toString(Type value)
{
    char buffer[NGREST_NUM_TO_STR_BUFF_SIZE];
    toCString(value, buffer, sizeof(buffer));
    return buffer;
}

template<typename Type>
std::string& toString(Type value, std::string& result)
{
    char buffer[NGREST_NUM_TO_STR_BUFF_SIZE];
    if (toCString(value, buffer, sizeof(buffer)))
        result = buffer;
    return result;
}

template<typename Type>
std::string toString(Type value, bool* ok)
{
    char buffer[NGREST_NUM_TO_STR_BUFF_SIZE];
    bool isOk = toCString(value, buffer, sizeof(buffer));
    if (ok)
        *ok = isOk;
    return buffer;
}

template<typename Type>
std::string& toString(Type value, std::string& result, bool* ok)
{
    char buffer[NGREST_NUM_TO_STR_BUFF_SIZE];
    bool isOk = toCString(value, buffer, sizeof(buffer));
    if (ok)
        *ok = isOk;
    if (isOk)
        result = buffer;
    return result;
}


template<typename Type>
std::string toHexString(Type value)
{
    char buffer[NGREST_NUM_TO_STR_BUFF_SIZE];
    toHexCString(value, buffer, sizeof(buffer));
    return buffer;
}


}

#endif // NGREST_UTILS_TOSTRING_H
