#include "Echo.h"

namespace ngrest {
namespace examples {

std::string Echo::echoPost(const std::string& text)
{
    return "You said: [" + text + "]";
}

std::string Echo::echoGet(const std::string& text)
{
    return "You said: [" + text + "]";
}

}
}
