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

#ifdef WIN32
#include <iostream>
#include "console.h"

#ifdef __MINGW32__
#ifndef COMMON_LVB_REVERSE_VIDEO
#define COMMON_LVB_REVERSE_VIDEO   0x4000
#endif
#ifndef COMMON_LVB_UNDERSCORE
#define COMMON_LVB_UNDERSCORE      0x8000
#endif
#endif

namespace ngrest
{

class ConsAttr
{
private:
    HANDLE consoleHandle;

public:
    ConsAttr():
        consoleHandle(GetStdHandle(STD_OUTPUT_HANDLE))
    {
    }

    ~ConsAttr()
    {
        CloseHandle(consoleHandle);
    }

    std::ostream& setAttribute(std::ostream& stream, WORD attr)
    {
        if (stream != std::cout)
        {
            return stream;
        }
        stream << std::flush;
        SetConsoleTextAttribute(consoleHandle, attr);
        return stream;
    }

    WORD getAttribute()
    {
        CONSOLE_SCREEN_BUFFER_INFO info;
        if (GetConsoleScreenBufferInfo(consoleHandle, &info)) {
            return info.wAttributes;
        } else {
            return 0xffffu;
        }
    }
};

ConsAttr consAttr;

std::ostream& colorDefault(std::ostream& stream)
{
    return consAttr.setAttribute(stream, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

std::ostream& colorBright(std::ostream& stream)
{
    return consAttr.setAttribute(stream, FOREGROUND_INTENSITY | consAttr.getAttribute());
}

std::ostream& colorDim(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() & ~ (FOREGROUND_INTENSITY & BACKGROUND_INTENSITY));
}

std::ostream& colorBrightDefault(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() & ~ (FOREGROUND_INTENSITY & BACKGROUND_INTENSITY));
}

std::ostream& colorFlashOn(std::ostream& stream)
{
    return consAttr.setAttribute(stream, BACKGROUND_INTENSITY | consAttr.getAttribute());
}

std::ostream& colorFlashOff(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() & ~ BACKGROUND_INTENSITY);
}

std::ostream& colorInverseOn(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() | COMMON_LVB_REVERSE_VIDEO);
}

std::ostream& colorInverseOff(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() & ~ COMMON_LVB_REVERSE_VIDEO);
}

std::ostream& colorUnderlineOn(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() | COMMON_LVB_UNDERSCORE);
}

std::ostream& colorUnderlineOff(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() & ~ COMMON_LVB_UNDERSCORE);
}

std::ostream& colorTextBlack(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() & ~ (FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE));
}

std::ostream& colorTextRed(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ (FOREGROUND_GREEN | FOREGROUND_BLUE)) | FOREGROUND_RED);
}

std::ostream& colorTextGreen(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ (FOREGROUND_RED | FOREGROUND_BLUE)) | FOREGROUND_GREEN);
}

std::ostream& colorTextBrown(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ FOREGROUND_BLUE) | FOREGROUND_RED | FOREGROUND_GREEN);
}

std::ostream& colorTextBlue(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ (FOREGROUND_RED | FOREGROUND_GREEN)) | FOREGROUND_BLUE);
}

std::ostream& colorTextMagenta(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ FOREGROUND_GREEN) | FOREGROUND_RED | FOREGROUND_BLUE);
}

std::ostream& colorTextCyan(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ FOREGROUND_RED) | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

std::ostream& colorTextGray(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}


std::ostream& colorBackgroundBlack(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() & ~ (BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE));
}

std::ostream& colorBackgroundRed(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ (BACKGROUND_GREEN | BACKGROUND_BLUE)) | BACKGROUND_RED);
}

std::ostream& colorBackgroundGreen(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ (BACKGROUND_RED | BACKGROUND_BLUE)) | BACKGROUND_GREEN);
}

std::ostream& colorBackgroundBrown(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ BACKGROUND_BLUE) | BACKGROUND_RED | BACKGROUND_GREEN);
}

std::ostream& colorBackgroundBlue(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ (BACKGROUND_RED | BACKGROUND_GREEN)) | BACKGROUND_BLUE);
}

std::ostream& colorBackgroundMagenta(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ BACKGROUND_GREEN) | BACKGROUND_RED | BACKGROUND_BLUE);
}

std::ostream& colorBackgroundCyan(std::ostream& stream)
{
    return consAttr.setAttribute(stream, (consAttr.getAttribute() & ~ BACKGROUND_RED) | BACKGROUND_GREEN | BACKGROUND_BLUE);
}

std::ostream& colorBackgroundGray(std::ostream& stream)
{
    return consAttr.setAttribute(stream, consAttr.getAttribute() | BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE);
}

} // namespace ngrest

#endif
