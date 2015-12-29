#ifndef NGREST_TESTSERVICE_H
#define NGREST_TESTSERVICE_H

#include <string>
#include <ngrest/common/Service.h>
#include <ngrest/common/Callback.h>
#include <ngrest/common/ObjectModel.h>

namespace ngrest {

struct Test
{
    struct Nested
    {
        bool b;
        Node node;
    };

    int a;
    std::string b;
    Nested n;
};

class TestService: public ngrest::Service
{
public:
    std::string echoSync(const std::string& value);
    void echoASync(const std::string& value, ngrest::Callback<const std::string&>& callback);
};

} // namespace ngrest

#endif // NGREST_TESTSERVICE_H
