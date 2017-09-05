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

#ifndef NGREST_FILELINE_H
#define NGREST_FILELINE_H

#include "static.h"

#ifdef _MSC_VER
#define NGREST_PATH_SEPARATOR_CH '\\'
#else
#define NGREST_PATH_SEPARATOR_CH '/'
#endif

#ifndef NGREST_FILE_LINE
#define NGREST_STRINGIZE(x) #x
#define NGREST_TO_STRING(x) NGREST_STRINGIZE(x)
#define NGREST_FILE_LINE __extension__ ({ \
    constexpr const int index = ::ngrest::static_max(0, \
        ::ngrest::static_lastIndexOf(__FILE__, NGREST_PATH_SEPARATOR_CH));\
    __FILE__ ":" NGREST_TO_STRING(__LINE__) " " + index; \
})
#endif

#endif
