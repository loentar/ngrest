#ifndef NGREST_UTILS_ERROR_H
#define NGREST_UTILS_ERROR_H

#include <string>
#include "ngrestutilsexport.h"

namespace ngrest {

//! get os error
class NGREST_UTILS_EXPORT Error
{
public:
    //! get error string by error no
    /*! \param  errorNo - error no
        \return error string
      */
    static std::string getError(long errorNo);

    //! get last error string
    /*! \return last error string
      */
    static std::string getLastError();

    //! get last library error string
    /*! \return last library error string
      */
    static std::string getLastLibraryError();
};

}

#endif // NGREST_UTILS_ERROR_H
