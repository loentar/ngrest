#ifdef WIN32
#include <windows.h>
#else
#include <errno.h>
#include <string.h>
#include <dlfcn.h>
#endif
#include "Error.h"

namespace ngrest {

std::string Error::getError(long errorNo)
{
#ifdef WIN32
    LPVOID buff = nullptr;
    DWORD count = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
                                 FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, errorNo, 0,
                                 reinterpret_cast<LPSTR>(&buff), 0, nullptr);

    if (!count || !buff) {
        return "<Unknown error>";
    } else {
        std::string error(reinterpret_cast<LPCSTR>(buff), static_cast<std::string::size_type>(count));
        LocalFree(buff);
        return error;
    }
#else
    return strerror(errorNo);
#endif
}

std::string Error::getLastError()
{
#ifdef WIN32
    return getError(GetLastError());
#else
    return getError(errno);
#endif
}

std::string Error::getLastLibraryError()
{
#ifdef WIN32
    return getError(GetLastError());
#else
    return dlerror();
#endif
}


}
