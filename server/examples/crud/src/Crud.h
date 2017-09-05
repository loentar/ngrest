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

#ifndef NGREST_EXAMPLES_CRUD_H
#define NGREST_EXAMPLES_CRUD_H

#include <string>
#include <vector>
#include <ngrest/common/Service.h>

namespace ngrest {
namespace examples {

//! Example on how to use a service in CRUD model
// *location: ngrest/examples/data
class Crud: public Service
{
public:
    //! get all identifiers from resource
    /*!
      example of request:
      http://server:port/ngrest/examples/data/
    */
    // *method: GET
    // *location: /
    std::vector<int> getIds();

    //! create new object in resource
    /*!
      example of request:
      http://server:port/ngrest/examples/data/1
      -- body -----------------------
      {
        "data": "Object #1"
      }
    */
    // *method: POST
    // *location: /{id}
    void create(int id, const std::string& data);

    //! create get object by identifier
    /*!
      example of request:
      http://server:port/ngrest/examples/data/1
    */
    // *method: GET
    // *location: /{id}
    std::string read(int id) const;

    //! update existing object in resource
    /*!
      example of request:
      http://server:port/ngrest/examples/data/1
      -- body -----------------------
      {
        "data": "Updated object #1"
      }
    */
    // *method: PUT
    // *location: /{id}
    void update(int id, const std::string& data);

    //! delete existing object by identifier
    /*!
      example of request:
      http://server:port/ngrest/examples/data/1
    */
    // *method: DELETE
    // *location: /{id}
    void del(int id);
};

}
}

#endif
