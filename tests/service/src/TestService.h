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

typedef Test TestTypedef;

enum ValType
{
    Zero,
    One,
    Two
};

class TestService: public ngrest::Service
{
public:
    bool get() const;
    // *resultElement: resultValue
    std::string echoSync(const std::string& value);
    void echoASync(const std::string& value, ngrest::Callback<const std::string&>& callback);
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
};

} // namespace ngrest

#endif // NGREST_TESTSERVICE_H
