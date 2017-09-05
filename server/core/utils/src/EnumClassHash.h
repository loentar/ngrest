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

#ifndef NGREST_UTILS_ENUMCLASSHASH_H
#define NGREST_UTILS_ENUMCLASSHASH_H

namespace ngrest {

struct EnumClassHash
{
    template <typename T>
    int operator()(T t) const
    {
        return static_cast<int>(t);
    }
};

} // namespace ngrest

#endif // NGREST_UTILS_ENUMCLASSHASH_H
