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

#include <ngrest/utils/Log.h>

#include "TestService.h"

namespace ngrest {

std::string TestService::echoSync(const std::string& value)
{
    return "You said " + value;
}

void TestService::echoASync(const std::string& value, ngrest::Callback<const std::string&>& callback)
{
    callback.success("You said " + value);
}

int TestService::add(int a, int b)
{
    return a + b;
}

void TestService::set(bool /*val*/)
{

}

void TestService::notify()
{
    LogDebug() << "Nofify";
}

Test TestService::test(const Test &arg)
{
    return arg;
}

std::list<std::string> TestService::templListStr(const std::list<std::string>& arg)
{
    return arg;
}

std::list<int> TestService::templList(const std::list<int>& arg)
{
    return arg;
}

std::vector<int> TestService::templVector(const std::vector<int>& arg)
{
    return arg;
}

std::list<Test> TestService::templListOfStruct(const std::list<Test>& arg)
{
    return arg;
}

std::list<TestTypedef> TestService::templListOfTypedef(const std::list<TestTypedef>& arg)
{
    return arg;
}

std::list<std::list<Test> > TestService::templListListStruct(const std::list<std::list<Test> >& arg)
{
    return arg;
}

std::list<std::list<TestTypedef> > TestService::templListListTypedef(const std::list<std::list<TestTypedef> >& arg)
{
    return arg;
}

std::list<std::list<int>> TestService::templListList(const std::list<std::list<int>>& arg)
{
    return arg;
}

std::list<std::list<std::string> > TestService::templListListString(const std::list<std::list<std::string> >& arg)
{
    return arg;
}

std::list<std::list<std::list<int> > > TestService::templListListList(const std::list<std::list<std::list<int> > >& arg)
{
    return arg;
}

std::map<int, std::string> TestService::templMapInt(const std::map<int, std::string>& arg)
{
    return arg;
}

std::map<ValType, std::string> TestService::templMapEnum(const std::map<ValType, std::string>& arg)
{
    return arg;
}

std::map<std::string, std::string> TestService::templMapStr(const std::map<std::string, std::string>& arg)
{
    return arg;
}

std::map<std::string, std::map<std::string, std::string> > TestService::templMapStrMapStrStr(const std::map<std::string, std::map<std::string, std::string> >& arg)
{
    return arg;
}

StringMap TestService::testTypedef(const StringMap& arg)
{
    return arg;
}

ValType TestService::testEnum(ValType arg)
{
    return arg;
}

Test::TestEnum TestService::testNestedEnum(Test::TestEnum arg)
{
    return arg;
}

Test::Nested TestService::testNestedStruct(Test::Nested arg)
{
    return arg;
}

bool TestService::get() const
{
    return true;
}

} // namespace ngrest

