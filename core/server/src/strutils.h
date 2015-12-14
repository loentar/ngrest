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

