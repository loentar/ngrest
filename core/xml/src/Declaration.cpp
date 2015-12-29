#include <sstream>
#include "Attribute.h"
#include "Exception.h"
#include "Declaration.h"

namespace ngrest {
namespace xml {

Declaration::Declaration():
    version("1.0"),
    encoding("UTF-8"),
    standalone(Standalone::Undefined)
{
}

Declaration::~Declaration()
{
}

const std::string& Declaration::getVersion() const
{
    return version;
}

void Declaration::setVersion(const std::string& version)
{
    this->version = version;
}

const std::string& Declaration::getEncoding() const
{
    return encoding;
}

void Declaration::setEncoding(const std::string& encoding)
{
    this->encoding = encoding;
}

Declaration::Standalone Declaration::getStandalone() const
{
    return standalone;
}

void Declaration::setStandalone(Declaration::Standalone standalone)
{
    this->standalone = standalone;
}

} // namespace xml
} // namespace ngrest
