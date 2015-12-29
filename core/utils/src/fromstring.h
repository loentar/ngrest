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
