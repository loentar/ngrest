#ifndef NGREST_UTILS_TOSTRING_H
#define NGREST_UTILS_TOSTRING_H

#include <string>
#include "tocstring.h"

namespace ngrest
{

template<typename Type>
std::string toString(Type value)
{
    char buffer[NUM_TO_STR_BUFF_SIZE];
    toCString(value, buffer, sizeof(buffer));
    return buffer;
}

template<typename Type>
std::string& toString(Type value, std::string& result)
{
    char buffer[NUM_TO_STR_BUFF_SIZE];
    if (toCString(value, buffer, sizeof(buffer)))
        result = buffer;
    return result;
}

template<typename Type>
std::string toString(Type value, bool* ok)
{
    char buffer[NUM_TO_STR_BUFF_SIZE];
    bool bIsOk = toCString(value, buffer, sizeof(buffer));
    if (ok)
        *ok = bIsOk;
    return buffer;
}

template<typename Type>
std::string& toString(Type value, std::string& result, bool* ok)
{
    char buffer[NUM_TO_STR_BUFF_SIZE];
    bool isOk = toCString(value, buffer, sizeof(buffer));
    if (ok)
        *ok = isOk;
    if (isOk)
        result = buffer;
    return result;
}


template<typename Type>
std::string ToHexString(Type value)
{
    char buffer[NUM_TO_STR_BUFF_SIZE];
    toHexCString(value, buffer, sizeof(buffer));
    return buffer;
}


}

#endif // NGREST_UTILS_TOSTRING_H
