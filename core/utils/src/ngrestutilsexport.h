
#ifndef NGREST_UTILS_EXPORT_H
#define NGREST_UTILS_EXPORT_H

#ifdef WIN32
  #ifdef _MSC_VER
    #pragma warning(disable: 4786 4251 4521)
  #endif

  #define NGREST_DLL_EXPORT __declspec(dllexport)
  #define NGREST_DLL_IMPORT __declspec(dllimport)

  #ifdef NGREST_UTILS_DLL_EXPORTS
    #define NGREST_UTILS_EXPORT NGREST_DLL_EXPORT
  #else
    #define NGREST_UTILS_EXPORT NGREST_DLL_IMPORT
  #endif
#else
  #define NGREST_UTILS_EXPORT
#endif

#ifndef NGREST_DEPRECATED
  #ifdef _MSC_VER
    #if _MSC_FULL_VER >= 140050320
      #define NGREST_DEPRECATED(Replacement) __declspec(deprecated("You are using deprecated API. Consider using " #Replacement " instead."))
    #else
      #define NGREST_DEPRECATED(Replacement) __declspec(deprecated)
    #endif
  #elif __GNUC__ >= 3
    #define NGREST_DEPRECATED(Replacement) __attribute__ ((deprecated))
  #else
    #define NGREST_DEPRECATED(Replacement)
  #endif
#endif

#endif // NGREST_UTILS_EXPORT_H

