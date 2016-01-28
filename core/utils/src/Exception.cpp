#include "Exception.h"

namespace ngrest {

Exception::Exception(const char* fileLine_, const char* function_, const std::string& description_):
    fileLine(fileLine_),
    function(function_),
    description(description_)
{
}

}

