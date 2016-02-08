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

#ifndef NGREST_UTILS_CONSOLE_H
#define NGREST_UTILS_CONSOLE_H

#include <iosfwd>
#include "ngrestutilsexport.h"

namespace ngrest
{

#ifndef WIN32

//! use default color
inline std::ostream& colorDefault(std::ostream& stream)
{
    return stream << "\033[0m";
}


//! bright color
inline std::ostream& colorBright(std::ostream& stream)
{
    return stream << "\033[1m";
}

//! dim color
inline std::ostream& colorDim(std::ostream& stream)
{
    return stream << "\033[2m";
}

//! default bright
inline std::ostream& colorBrightDefault(std::ostream& stream)
{
    return stream << "\033[22m";
}

//! enable flashing
inline std::ostream& colorFlashOn(std::ostream& stream)
{
    return stream << "\033[5m";
}

//! disable flashing
inline std::ostream& colorFlashOff(std::ostream& stream)
{
    return stream << "\033[25m";
}

//! enable inversion
inline std::ostream& colorInverseOn(std::ostream& stream)
{
    return stream << "\033[7m";
}

//! disable inversion
inline std::ostream& colorInverseOff(std::ostream& stream)
{
    return stream << "\033[27m";
}

//! enable underlining
inline std::ostream& colorUnderlineOn(std::ostream& stream)
{
    return stream << "\033[4m";
}

//! disable underlining
inline std::ostream& colorUnderlineOff(std::ostream& stream)
{
    return stream << "\033[24m";
}


// text color
//! black text
inline std::ostream& colorTextBlack(std::ostream& stream)
{
    return stream << "\033[30m";
}

//! red text
inline std::ostream& colorTextRed(std::ostream& stream)
{
    return stream << "\033[31m";
}

//! green text
inline std::ostream& colorTextGreen(std::ostream& stream)
{
    return stream << "\033[32m";
}

//! brown text
inline std::ostream& colorTextBrown(std::ostream& stream)
{
    return stream << "\033[33m";
}

//! blue text
inline std::ostream& colorTextBlue(std::ostream& stream)
{
    return stream << "\033[34m";
}

//! magenta text
inline std::ostream& colorTextMagenta(std::ostream& stream)
{
    return stream << "\033[35m";
}

//! cyan text
inline std::ostream& colorTextCyan(std::ostream& stream)
{
    return stream << "\033[36m";
}

//! gray text
inline std::ostream& colorTextGray(std::ostream& stream)
{
    return stream << "\033[37m";
}


// background color
//! black background
inline std::ostream& colorBackgroundBlack(std::ostream& stream)
{
    return stream << "\033[40m";
}

//! red background
inline std::ostream& colorBackgroundRed(std::ostream& stream)
{
    return stream << "\033[41m";
}

//! green background
inline std::ostream& colorBackgroundGreen(std::ostream& stream)
{
    return stream << "\033[42m";
}

//! brown background
inline std::ostream& colorBackgroundBrown(std::ostream& stream)
{
    return stream << "\033[43m";
}

//! blue background
inline std::ostream& colorBackgroundBlue(std::ostream& stream)
{
    return stream << "\033[44m";
}

//! magenta background
inline std::ostream& colorBackgroundMagenta(std::ostream& stream)
{
    return stream << "\033[45m";
}

//! cyan background
inline std::ostream& colorBackgroundCyan(std::ostream& stream)
{
    return stream << "\033[46m";
}

//! gray background
inline std::ostream& colorBackgroundGray(std::ostream& stream)
{
    return stream << "\033[47m";
}

#else // WIN32

//! use default color
NGREST_UTILS_EXPORT std::ostream& colorDefault(std::ostream& stream);

//! bright color
NGREST_UTILS_EXPORT std::ostream& colorBright(std::ostream& stream);
//! dim color
NGREST_UTILS_EXPORT std::ostream& colorDim(std::ostream& stream);
//! default bright
NGREST_UTILS_EXPORT std::ostream& colorBrightDefault(std::ostream& stream);
//! enable flashing
NGREST_UTILS_EXPORT std::ostream& colorFlashOn(std::ostream& stream);
//! disable flashing
NGREST_UTILS_EXPORT std::ostream& colorFlashOff(std::ostream& stream);
//! enable inversion
NGREST_UTILS_EXPORT std::ostream& colorInverseOn(std::ostream& stream);
//! disable inversion
NGREST_UTILS_EXPORT std::ostream& colorInverseOff(std::ostream& stream);
//! enable underlining
NGREST_UTILS_EXPORT std::ostream& colorUnderlineOn(std::ostream& stream);
//! disable underlining
NGREST_UTILS_EXPORT std::ostream& colorUnderlineOff(std::ostream& stream);

// text color
//! black text
NGREST_UTILS_EXPORT std::ostream& colorTextBlack(std::ostream& stream);
//! red text
NGREST_UTILS_EXPORT std::ostream& colorTextRed(std::ostream& stream);
//! green text
NGREST_UTILS_EXPORT std::ostream& colorTextGreen(std::ostream& stream);
//! brown text
NGREST_UTILS_EXPORT std::ostream& colorTextBrown(std::ostream& stream);
//! blue text
NGREST_UTILS_EXPORT std::ostream& colorTextBlue(std::ostream& stream);
//! magenta text
NGREST_UTILS_EXPORT std::ostream& colorTextMagenta(std::ostream& stream);
//! cyan text
NGREST_UTILS_EXPORT std::ostream& colorTextCyan(std::ostream& stream);
//! gray text
NGREST_UTILS_EXPORT std::ostream& colorTextGray(std::ostream& stream);

// background color
//! black background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundBlack(std::ostream& stream);
//! red background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundRed(std::ostream& stream);
//! green background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundGreen(std::ostream& stream);
//! brown background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundBrown(std::ostream& stream);
//! blue background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundBlue(std::ostream& stream);
//! magenta background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundMagenta(std::ostream& stream);
//! cyan background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundCyan(std::ostream& stream);
//! gray background
NGREST_UTILS_EXPORT std::ostream& colorBackgroundGray(std::ostream& stream);

#endif


}  // namespace ngrest

#endif
