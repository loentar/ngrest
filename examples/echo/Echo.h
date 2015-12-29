#ifndef NGREST_EXAMPLES_ECHO_H
#define NGREST_EXAMPLES_ECHO_H

#include <string>
#include <ngrest/Service.h>

namespace ngrest {
namespace examples {
namespace echo {

// by default exposes Echo service relative to base URL: http://server:port/ngrest/examples/echo/Echo
class Echo: public Service
{
public:
    /*
      example of POST request:
      http://server:port/ngrest/examples/echo/Echo
      -- body -----------------------
      {
        "echo": {
          str: "Hello Ngrest!"
        }
      }
      -- end body -------------------
    */
    std::string echo(const std::string& str);
};

}
}
}

#endif
