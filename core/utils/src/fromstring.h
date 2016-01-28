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

#include <string>
#include "fromcstring.h"

#ifndef NGREST_UTILS_FROMSTRING_H
#define NGREST_UTILS_FROMSTRING_H

namespace ngrest {

  template<typename Type>
  inline Type& fromString(const std::string& string, Type& value)
  {
    fromCString(string.c_str(), value);
    return value;
  }

  template<typename Type>
  inline Type& fromString(const std::string& string, Type& value, bool* ok)
  {
    bool res = fromCString(string.c_str(), value);
    if (ok)
      *ok = res;
    return value;
  }

  template<typename Type>
  inline Type& fromHexString(const std::string& string, Type& value)
  {
    fromHexCString(string.c_str(), value);
    return value;
  }

}

#endif // NGREST_UTILS_FROMSTRING_H
