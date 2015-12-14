#ifndef _NGREST_UTILS_LOGSTREAM_H_
#define _NGREST_UTILS_LOGSTREAM_H_

#include <ostream>
#include "ngrestutilsexport.h"

namespace ngrest
{
// wsdl compat
typedef char byte;
typedef unsigned char unsignedByte;

//! log stream - log output helper (internal use only)
class NGREST_UTILS_EXPORT LogStream
{
public:
    inline LogStream(std::ostream* stream_, bool writeEol_ = true):
        stream(stream_), writeEol(writeEol_)
    {
    }

    inline ~LogStream()
    {
        if (stream && writeEol)
            *stream << std::endl;
    }

    inline LogStream& operator<<(bool value)
    {
        if (stream)
            // override std::boolalpha
            *stream << (value ? "true" : "false");
        return *this;
    }

    // for complex types
    template<typename Type>
    inline LogStream& operator<<(Type value)
    {
        if (stream)
            *stream << value;
        return *this;
    }

private:
    friend LogStream& logEolOff(LogStream& rLogStream);
    friend LogStream& LogEolOn(LogStream& rLogStream);

private:
    std::ostream* stream;
    bool          writeEol;
};

//! disable carriage return
inline LogStream& logEolOff(LogStream& rLogStream)
{
    rLogStream.writeEol = false;
    return rLogStream;
}

//! enable carriage return
inline LogStream& LogEolOn(LogStream& rLogStream)
{
    rLogStream.writeEol = true;
    return rLogStream;
}

} // namespace ngrest

#endif // _NGREST_UTILS_LOGSTREAM_H_
