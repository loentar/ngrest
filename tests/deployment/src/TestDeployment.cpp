#include "TestDeployment.h"

namespace ngrest {

std::string TestDeployment::echoSync(const std::string& value)
{
    return "You said " + value;
}

void TestDeployment::echoASync(const std::string& value, ngrest::Callback<const std::string&>& callback)
{
    callback.success("You said " + value);
}

} // namespace ngrest

