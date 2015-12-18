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
    void echoASync(const std::string& value, ngrest::Callback<const std::string&>* callback);
};

} // namespace ngrest

#endif // NGREST_TESTDEPLOYMENT_H
