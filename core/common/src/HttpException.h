#ifndef HTTPEXCEPTION_H
#define HTTPEXCEPTION_H

#include <ngrest/utils/Exception.h>
#include "HttpStatus.h"


//! throw HTTP exception
#define NGREST_THROW_HTTP(DESCRIPTION, STATUS) \
    throw ::ngrest::HttpException(NGREST_FILE_LINE, __FUNCTION__, DESCRIPTION, STATUS);

//! assert HTTP expression
#define NGREST_ASSERT_HTTP(EXPRESSION, STATUS, DESCRIPTION) \
    if (NGREST_UNLIKELY(!(EXPRESSION))) NGREST_THROW_HTTP(DESCRIPTION, STATUS)


namespace ngrest {

//! HTTP exception
class NGREST_UTILS_EXPORT HttpException: public Exception
{
public:
    //! exception constructor
    /*! \param  fileLine source file name and line number
        \param  function function signature
        \param  description description
        \param  status HTTP status code
      */
    inline HttpException(const char* fileLine, const char* function, const std::string& description,
                  HttpStatus status):
        Exception(fileLine, function, description), httpStatus(status)
    {
    }

    inline HttpStatus getHttpStatus() const
    {
        return httpStatus;
    }

private:
    const HttpStatus httpStatus;
};

}

#endif // HTTPEXCEPTION_H
