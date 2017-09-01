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

#ifndef NGREST_STATIC_H
#define NGREST_STATIC_H

namespace ngrest {

template <typename T>
constexpr int static_max(T a, T b)
{
    return a > b ? a : b;
}

template <int size>
constexpr int static_strlen(const char(&)[size])
{
    return size - 1;
}

template <int size>
constexpr int static_lastIndexOf(const char(&str)[size], char ch, int pos)
{
    return (pos == -1) ? -1 : ((str[pos] == ch) ? (pos + 1) : static_lastIndexOf(str, ch, pos - 1));
}

template <int size>
constexpr int static_lastIndexOf(const char(&str)[size], char ch)
{
    return static_lastIndexOf(str, ch, size - 1);
}

}

#endif

