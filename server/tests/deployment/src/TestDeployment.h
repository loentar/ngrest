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

#ifndef NGREST_TESTDEPLOYMENT_H
#define NGREST_TESTDEPLOYMENT_H

#include <string>
#include <ngrest/common/Service.h>
#include <ngrest/common/Callback.h>

namespace ngrest {

class TestDeployment: public ngrest::Service
{
public:
    std::string echoSync(const std::string& value);
    void echoASync(const std::string& value, ngrest::Callback<const std::string&>& callback);
};

} // namespace ngrest

#endif // NGREST_TESTDEPLOYMENT_H
