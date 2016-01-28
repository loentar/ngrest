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
    return size;
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

