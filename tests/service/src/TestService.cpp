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

#ifdef NGREST_THREAD_LOCK
#include <chrono>
#include <thread>
#endif
#include <ngrest/utils/Log.h>
#include <ngrest/engine/Handler.h>

#include "TestService.h"

namespace ngrest {

bool TestService::get() const
{
    return true;
}

std::string TestService::echoSync(const std::string& value)
{
    return "You said " + value;
}

void TestService::echoASync(const std::string& value, ngrest::Callback<const std::string&>& callback)
{
    // take callback by reference because it's allocated in mempool
    // take argument(s) by value because it's allocated in stack

#ifdef NGREST_THREAD_LOCK
    std::thread([&callback, value]{
        // perform some long synchronous operation
        std::this_thread::sleep_for(std::chrono::seconds(1));
#endif
        // post to event loop
        Handler::post([&callback, value]{
            // this will be executed from main thread
            callback.success("You said " + value);
        });
#ifdef NGREST_THREAD_LOCK
    })
    .detach();
#endif
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

StringList TestService::templListOfTypedefList(const StringList& arg)
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

ngrest::Nullable<int> TestService::ptrInt(ngrest::Nullable<int> arg)
{
    return arg;
}

ngrest::Nullable<int> TestService::ptrIntConst(const ngrest::Nullable<int>& arg)
{
    return arg;
}

ngrest::Nullable<std::string> TestService::ptrString(ngrest::Nullable<std::string> arg)
{
    return arg;
}

ngrest::Nullable<ValType> TestService::ptrEnum(ngrest::Nullable<ValType> arg)
{
    return arg;
}

ngrest::Nullable<Test> TestService::ptrStruct(ngrest::Nullable<Test> arg)
{
    return arg;
}

ngrest::Nullable<std::list<Test> > TestService::ptrStructList(ngrest::Nullable<std::list<Test> > arg)
{
    return arg;
}

TestPtr TestService::ptrNull()
{
    return TestPtr {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr
    };

}

TestPtr TestService::ptrNotNull()
{
    return TestPtr {
        1, // init via reference
        Two, // init via reference
        std::string("Hello!"), // init via reference
        new Test { // init via ptr
            3, "four", Test::Here, {true}
        },
        std::list<std::string> {"aaa", "bbb", "ccc"}, // init via reference
        new std::map<int, std::string> {{1, "qqq"}, {2, "www"}} // init via ptr
    };
}

TestPtr TestService::ptrTest(const TestPtr& arg)
{
    return arg;
}

ngrest::Nullable<TestPtr> TestService::ptrTestNull(const ngrest::Nullable<TestPtr>& arg)
{
    return arg;
}


// inline result

bool TestService::getInline() const
{
    return true;
}

std::string TestService::echoSyncInline(const std::string &value)
{
    return "You said " + value;
}

int TestService::addInline(int a, int b)
{
    return a + b;
}

void TestService::notifyInline()
{
}

Test TestService::testInline(const Test &arg)
{
    return arg;
}

std::list<std::string> TestService::templListStrInline(const std::list<std::string> &arg)
{
    return arg;
}

std::vector<int> TestService::templVectorInline(const std::vector<int> &arg)
{
    return arg;
}

StringList TestService::templListOfTypedefListInline(const StringList &arg)
{
    return arg;
}

std::list<std::list<int> > TestService::templListListInline(const std::list<std::list<int> > &arg)
{
    return arg;
}

std::map<int, std::string> TestService::templMapIntInline(const std::map<int, std::string> &arg)
{
    return arg;
}

StringMap TestService::testTypedefInline(const StringMap &arg)
{
    return arg;
}

ValType TestService::testEnumInline(ValType arg)
{
    return arg;
}

Test::TestEnum TestService::testNestedEnumInline(Test::TestEnum arg)
{
    return arg;
}

Test::Nested TestService::testNestedStructInline(Test::Nested arg)
{
    return arg;
}

ngrest::Nullable<int> TestService::ptrIntInline(ngrest::Nullable<int> arg)
{
    return arg;
}

std::string TestService::echo(const std::string& value)
{
    return value;
}

std::string TestService::echoPost(const std::string& value)
{
    return value;
}

} // namespace ngrest

