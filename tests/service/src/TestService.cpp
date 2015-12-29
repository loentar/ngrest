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

} // namespace ngrest

