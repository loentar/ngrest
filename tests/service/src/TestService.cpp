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

