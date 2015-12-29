#ifndef NGREST_UTILS_STRINGUTILS_H
#define NGREST_UTILS_STRINGUTILS_H

#include <string>

namespace ngrest {

//! perform replace in string
/*! \param  where - string where search and replace should be performed
    \param  what - what data replace
    \param  with - replace with data
    \param  all - true - replace all, false - replace only first
    \return number of replaces made
    */
inline unsigned stringReplace(std::string& where, const std::string& what,
                              const std::string& with, bool all = false)
{
    unsigned count = 0;
    std::string::size_type pos = 0;
    while ((pos = where.find(what, pos)) != std::string::npos) {
        where.replace(pos, what.size(), with);
        ++count;
        if (!all)
            break;
        pos += with.size();
    }

    return count;
}

//! remove leading whitespaces
/*! \param  string - string
    \param  whitespace - whitespace symbols
    */
inline void stringTrimLeft(std::string& str, const char* whitespace = " \n\r\t")
{
    if (whitespace) {
        std::string::size_type pos = str.find_first_not_of(whitespace);
        if (pos)
            str.erase(0, pos);
    }
}

//! remove trailing whitespaces
/*! \param  string - string
    \param  whitespace - space symbols
    */
inline void stringTrimRight(std::string& str, const char* whitespace = " \n\r\t")
{
    if (whitespace) {
        std::string::size_type pos = str.find_last_not_of(whitespace);
        if (pos != std::string::npos)
            str.erase(pos + 1);
    }
}

//! remove leading and trailing whitespaces
/*! \param  string - string
    \param  whitespace - space symbols
    */
inline void stringTrim(std::string& str, const char* whitespace = " \n\r\t")
{
    stringTrimLeft(str, whitespace);
    stringTrimRight(str, whitespace);
}

}

#endif // NGREST_UTILS_STRINGUTILS_H
