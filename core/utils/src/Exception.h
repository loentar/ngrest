#ifndef NGREST_UTILS_EXCEPTION_H
#define NGREST_UTILS_EXCEPTION_H

#include <string>
#include <exception>
#include "ngrestutilsexport.h"

#ifndef NGREST_FILE_LINE
#define NGREST_STRINGIZE(x) #x
#define NGREST_TO_STRING(x) NGREST_STRINGIZE(x)
#define NGREST_FILE_LINE __FILE__ ":" NGREST_TO_STRING(__LINE__) " "
#endif

//! throw specific exception
#define NGREST_THROW(EXCEPTION_CLASS, DESCRIPTION) \
    throw EXCEPTION_CLASS(NGREST_FILE_LINE, __FUNCTION__, DESCRIPTION);

//! throw assert exception
#define NGREST_THROW_ASSERT(DESCRIPTION) \
    throw ::ngrest::AssertException(NGREST_FILE_LINE, __FUNCTION__, DESCRIPTION);

#ifdef __GNUC__
#define NGREST_LIKELY(EXPRESSION) __builtin_expect(!!(EXPRESSION), true)
#define NGREST_UNLIKELY(EXPRESSION) __builtin_expect(!!(EXPRESSION), false)
#else
#define NGREST_LIKELY(EXPRESSION) (EXPRESSION)
#define NGREST_UNLIKELY(EXPRESSION) (EXPRESSION)
#endif

//! assert expression
#define NGREST_ASSERT(EXPRESSION, DESCRIPTION) \
    if (NGREST_UNLIKELY(!(EXPRESSION))) NGREST_THROW_ASSERT(DESCRIPTION)

//! assert parameter
#define NGREST_ASSERT_PARAM(EXPRESSION) \
    if (NGREST_UNLIKELY(!(EXPRESSION))) NGREST_THROW_ASSERT("Invalid argument: (" #EXPRESSION ")")

//! assert parameter
#define NGREST_ASSERT_NULL(EXPRESSION) \
    if (NGREST_UNLIKELY(!(EXPRESSION))) NGREST_THROW_ASSERT(#EXPRESSION " is null!")

//! debug assert expression
#ifdef _DEBUG
#define NGREST_DEBUG_ASSERT(EXPRESSION, DESCRIPTION) \
    if (!(EXPRESSION)) \
    NGREST_THROW_ASSERT(DESCRIPTION + std::string(". Debug assertion failed: (" #EXPRESSION ")"))
#else
#define NGREST_DEBUG_ASSERT(EXPRESSION, DESCRIPTION)
#endif

//! catch all exceptions
#define NGREST_CATCH_ALL\
    catch (const ::ngrest::Exception& exception) {\
        ::ngrest::LogError() << "Exception: \n" << exception.what();\
    } catch (const std::exception& exception) {\
        ::ngrest::LogError() << "std::exception: \n" << exception.what();\
    } catch (...) {\
        ::ngrest::LogError() << "unknown exception"; \
    }

//! catch all exceptions and write description
#define NGREST_CATCH_ALL_DESCR(DESCRIPTION)\
    catch (const ::ngrest::Exception& exception) {\
        ::ngrest::LogError() << (DESCRIPTION) << "\nException: \n" << exception.what();\
    } catch (const std::exception& exception) {\
        ::ngrest::LogError() << (DESCRIPTION) << "\nstd::exception: \n" << exception.what();\
    } catch (...) {\
        ::ngrest::LogError() << (DESCRIPTION) << "\nunknown exception"; \
    }

namespace ngrest {

//! base ngrest exception class
class NGREST_UTILS_EXPORT Exception: public std::exception
{
public:
    //! exception constructor
    /*! \param  fileLine source file name and line number
        \param  function function signature
        \param  description description
      */
    Exception(const char* fileLine, const char* function, const std::string& description);

    //! destructor
    inline virtual ~Exception() throw()
    {
    }

    //! get description of exception thrown
    /*! \return description
      */
    inline virtual const char* what() const throw()
    {
        return description.c_str();
    }

    //! get description of exception thrown
    /*! \return description
      */
    inline virtual const std::string& strWhat() const throw()
    {
        return description;
    }

    //! get source file and line where exception was raised
    /*! \return source file and line
      */
    inline const char* getFileLine() const
    {
        return fileLine;
    }

    //! get function name where exception was raised
    /*! \return function name
      */
    inline const char* getFunction() const
    {
        return function;
    }

private:
    const char* fileLine;    //!< stored statically
    const char* function;    //!< stored statically
    std::string description; //!< description
};

//! assert exception class
class NGREST_UTILS_EXPORT AssertException: public Exception
{
public:
    //! exception constructor
    /*! \param  fileLine - source file name and line number
        \param  function - function signature
        \param  description - description
      */
    inline AssertException(const char* fileLine, const char* function, const std::string& description):
        Exception(fileLine, function, description)
    {
    }
};

}

#endif // NGREST_UTILS_EXCEPTION_H
