#ifndef NGREST_UTILS_DYNAMICLIBRARY_H
#define NGREST_UTILS_DYNAMICLIBRARY_H

#include <string>
#include "ngrestutilsexport.h"

#ifdef WIN32
#define NGREST_LIBRARY_PREFIX ""
#define NGREST_LIBRARY_EXT ".dll"
#else
#ifdef __APPLE__
#define NGREST_LIBRARY_PREFIX "lib"
#define NGREST_LIBRARY_EXT ".dylib"
#else
#define NGREST_LIBRARY_PREFIX "lib"
#define NGREST_LIBRARY_EXT ".so"
#endif
#endif

namespace ngrest {

//! dynamic library
class NGREST_UTILS_EXPORT DynamicLibrary
{
public:
    //! constructor
    DynamicLibrary();

    //! destructor
    virtual ~DynamicLibrary();

    //! load dynamic library
    /*! \param  libName library name
        \param  raw if true libName contains full path to library
        */
    void load(const std::string& libName, bool raw = false);

    //! get library name
    /*! \return library name
    */
    const std::string& name() const;

    //! get pointer to library symbol
    /*! \param  symbol symbol name
        \return pointer to symbol
    */
    void* getSymbol(const std::string& symbol) const;

    //! unload library
    void unload();

    //! reload library
    void reload();

private:
    void* handle = nullptr; //!< library handle
    std::string libName;    //!< library name
    bool raw = false;       //!< libName is full
};

} // namespace ngrest

#endif // NGREST_UTILS_DYNAMICLIBRARY_H
