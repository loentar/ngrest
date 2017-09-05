/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

#ifndef NGREST_UTILS_LOGSTREAM_H
#define NGREST_UTILS_LOGSTREAM_H

#include <ostream>
#include "console.h"
#include "ngrestutilsexport.h"

namespace ngrest {

// wsdl compat
typedef char byte;
typedef unsigned char unsignedByte;

//! log stream - log output helper (internal use only)
class NGREST_UTILS_EXPORT LogStream
{
public:
    inline LogStream(std::ostream* stream_, bool color_, bool writeEol_ = true):
        stream(stream_), color(color_), writeEol(writeEol_)
    {
    }

    inline ~LogStream()
    {
        if (stream) {
            if (color)
                *stream << colorDefault;
            if (writeEol)
                *stream << std::endl;
        }
    }

    inline LogStream& operator<<(bool value)
    {
        if (stream)
            // override std::boolalpha
            *stream << (value ? "true" : "false");
        return *this;
    }

    inline LogStream& write(const char* buff, int size)
    {
        if (stream)
            stream->write(buff, size);
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
    friend LogStream& logEolOn(LogStream& logStream);

private:
    std::ostream* stream;
    bool          color;
    bool          writeEol;
};

//! disable carriage return
inline LogStream& logEolOff(LogStream& logStream)
{
    logStream.writeEol = false;
    return logStream;
}

//! enable carriage return
inline LogStream& logEolOn(LogStream& logStream)
{
    logStream.writeEol = true;
    return logStream;
}

} // namespace ngrest

#endif // NGREST_UTILS_LOGSTREAM_H
