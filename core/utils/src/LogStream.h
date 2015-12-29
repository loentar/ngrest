#ifndef NGREST_UTILS_LOGSTREAM_H
#define NGREST_UTILS_LOGSTREAM_H

#include <ostream>
#include "ngrestutilsexport.h"

namespace ngrest {

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
    friend LogStream& logEolOff(LogStream& logStream);
    friend LogStream& LogEolOn(LogStream& logStream);

private:
    std::ostream* stream;
    bool          writeEol;
};

//! disable carriage return
inline LogStream& logEolOff(LogStream& logStream)
{
    logStream.writeEol = false;
    return logStream;
}

//! enable carriage return
inline LogStream& LogEolOn(LogStream& logStream)
{
    logStream.writeEol = true;
    return logStream;
}

} // namespace ngrest

#endif // NGREST_UTILS_LOGSTREAM_H
