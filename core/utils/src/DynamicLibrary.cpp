#ifndef WIN32
#include <dlfcn.h>
#else
#include <windows.h>
#endif
#include "Exception.h"
#include "Error.h"
#include "DynamicLibrary.h"

namespace ngrest {

DynamicLibrary::DynamicLibrary()
{
}

DynamicLibrary::~DynamicLibrary()
{
}

void DynamicLibrary::load(const std::string& libName, bool raw /*= false*/)
{
    NGREST_ASSERT(!handle, "Library is already loaded");

    if (raw) {
#ifdef WIN32
        handle = LoadLibraryA(libName.c_str());
#else
        handle = dlopen(libName.c_str(), RTLD_LAZY);
#endif
    } else {
#ifdef WIN32
        handle = LoadLibraryA((libName + NGREST_LIBRARY_EXT).c_str());
#else
        std::string::size_type pos = libName.find_last_of('/');
        if (pos == std::string::npos) {
            handle = dlopen((NGREST_LIBRARY_PREFIX + libName + NGREST_LIBRARY_EXT).c_str(), RTLD_LAZY);
        } else {
            handle = dlopen((libName.substr(pos) + NGREST_LIBRARY_PREFIX +
                             libName.substr(pos + 1) + NGREST_LIBRARY_EXT).c_str(), RTLD_LAZY);
        }
#endif
    }

    NGREST_ASSERT(handle, "Failed load library [" + libName + "]: " + Error::getLastLibraryError());
    this->libName = libName;
    this->raw = raw;
}

const std::string& DynamicLibrary::name() const
{
    NGREST_ASSERT(handle, "Library is not loaded");
    return libName;
}

void* DynamicLibrary::getSymbol(const std::string& symbol) const
{
    NGREST_ASSERT(handle, "Library is not loaded");

#ifdef WIN32
    void* result = reinterpret_cast<void*>(GetProcAddress(reinterpret_cast<HMODULE>(handle), symbol.c_str()));
#else
    void* result = dlsym(handle, symbol.c_str());
#endif

    NGREST_ASSERT(result, "Failed to get symbol [" + symbol + "]: " + Error::getLastLibraryError());
    return result;
}

void DynamicLibrary::unload()
{
    NGREST_ASSERT(handle, "Library is not loaded");
#ifdef WIN32
    FreeLibrary(reinterpret_cast<HMODULE>(handle));
#else
    dlclose(handle);
#endif
    handle = nullptr;
}

void DynamicLibrary::reload()
{
    unload();
    load(libName, raw);
}

}
