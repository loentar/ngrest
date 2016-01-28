#ifndef NGREST_FILELINE_H
#define NGREST_FILELINE_H

#include "static.h"

#ifdef _MSC_VER
#define NGREST_PATH_SEPARATOR_CH '\\'
#else
#define NGREST_PATH_SEPARATOR_CH '/'
#endif

#ifndef NGREST_FILE_LINE
#define NGREST_STRINGIZE(x) #x
#define NGREST_TO_STRING(x) NGREST_STRINGIZE(x)
#define NGREST_FILE_LINE __extension__ ({ \
    constexpr const int index = ::ngrest::static_max(0, \
        ::ngrest::static_lastIndexOf(__FILE__, NGREST_PATH_SEPARATOR_CH));\
    __FILE__ ":" NGREST_TO_STRING(__LINE__) " " + index; \
})
#endif

#endif
