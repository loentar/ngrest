#ifndef NGREST_EXAMPLES_ECHO_H
#define NGREST_EXAMPLES_ECHO_H

#include <string>
#include <ngrest/common/Service.h>

namespace ngrest {
namespace examples {

//! Echo service example
/*! by default exposes Echo service relative to base URL: http://server:port/ngrest/examples/Echo */
class Echo: public Service
{
public:
    //! a simple operation "echo"
    /*!
      example of POST request:
      http://server:port/ngrest/examples/Echo/echo
      -- body -----------------------
      {
        "str": "Hello ngrest!"
      }
      -- end body -------------------
    */
    // *method: POST
    // *location: echo
    std::string echoPost(const std::string& text);

    //! a simple operation "echo"
    /*!
      example of GET request:
      http://server:port/ngrest/examples/Echo?text=Hello%20World!
    */
    // *method: GET
    // *location: echo?text={text}
    std::string echoGet(const std::string& text);
};

}
}

#endif
