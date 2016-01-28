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

#ifndef NGREST_TESTSERVICE_H
#define NGREST_TESTSERVICE_H

#include <string>
#include <list>
#include <vector>
#include <map>
#include <ngrest/common/Service.h>
#include <ngrest/common/Callback.h>
#include <ngrest/common/ObjectModel.h>

namespace ngrest {

struct Test
{
    struct Nested
    {
        bool b;
//        Node* node;
    };

    enum TestEnum
    {
        Some,
        Values,
        Here
    };

    int a;
    std::string b;
    Nested n;
};

struct TestChild: public Test
{
    bool isSomething;
};

typedef Test TestTypedef;
typedef std::map<std::string, std::string> StringMap;

enum ValType
{
    Zero,
    One,
    Two
};

// *location: ngrest/test
class TestService: public ngrest::Service
{
public:
    bool get() const;
    // *resultElement: resultValue
    std::string echoSync(const std::string& value);
    void echoASync(const std::string& value, ngrest::Callback<const std::string&>& callback);
    // default location is: add?a={a}&b={b}
    int add(int a, int b);
    void set(bool val);
    void notify();

    // *method: PUT
    // *location: theTest
    Test test(const Test& arg);

    // serialized as array
    std::list<std::string> templListStr(const std::list<std::string>& arg);
    std::list<int> templList(const std::list<int>& arg);
    std::vector<int> templVector(const std::vector<int>& arg);
    std::list<Test> templListOfStruct(const std::list<Test>& arg);
    std::list<TestTypedef> templListOfTypedef(const std::list<TestTypedef>& arg);


    std::list<std::list<int>> templListList(const std::list<std::list<int>>& arg);
    std::list<std::list<std::string>> templListListString(const std::list<std::list<std::string>>& arg);
    std::list<std::list<Test>> templListListStruct(const std::list<std::list<Test>>& arg);
    std::list<std::list<TestTypedef>> templListListTypedef(const std::list<std::list<TestTypedef>>& arg);


    std::list<std::list<std::list<int>>> templListListList(const std::list<std::list<std::list<int>>>& arg);

    // serialized as object
    std::map<int, std::string> templMapInt(const std::map<int, std::string>& arg);
    std::map<ValType, std::string> templMapEnum(const std::map<ValType, std::string>& arg);
    std::map<std::string, std::string> templMapStr(const std::map<std::string, std::string>& arg);
    std::map<std::string, std::map<std::string, std::string>> templMapStrMapStrStr(const std::map<std::string, std::map<std::string, std::string>>& arg);

    StringMap testTypedef(const StringMap& arg);
    ValType testEnum(ValType arg);
    Test::TestEnum testNestedEnum(Test::TestEnum arg);
    Test::Nested testNestedStruct(Test::Nested arg);
};

} // namespace ngrest

#endif // NGREST_TESTSERVICE_H
