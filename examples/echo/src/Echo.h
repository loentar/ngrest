/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

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
