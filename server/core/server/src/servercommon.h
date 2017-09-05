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

#ifndef NGREST_SERVERCOMMON_H
#define NGREST_SERVERCOMMON_H

#include <map>
#include <string>

#ifdef WIN32
#include <winsock2.h>
#endif

namespace ngrest {

#ifndef WIN32
typedef int Socket;
#define NGREST_SOCKET_ERROR -1
#else
typedef SOCKET Socket;
#define NGREST_SOCKET_ERROR static_cast<SOCKET>(SOCKET_ERROR)
#endif

typedef std::map<std::string, std::string> StringMap;

}

#endif // NGREST_SERVERCOMMON_H

