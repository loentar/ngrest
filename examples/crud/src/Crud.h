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
#include <ngrest/common/Nullable.h>

namespace ngrest {
namespace examples {

/**
 * @brief User user
 */
struct User
{
    ngrest::Nullable<int> id;  //!< user id
    std::string name;          //!< user name
    std::string email;         //!< user email
};


/**
 * @brief User user for PATCH request
 */
struct UserPatch
{
    ngrest::Nullable<std::string> name;   //!< user name, don't update if not set
    ngrest::Nullable<std::string> email;  //!< user email, don't update if not set
};


//! Example on how to use a service in CRUD model
// *location: ngrest/examples/users
class Crud: public Service
{
public:
    //! get all identifiers from resource
    /*!
      example of request:
      GET http://server:port/ngrest/examples/users/
    */
    // *method: GET
    // *location: /
    std::vector<User> getAll();

    //! create new object in resource
    /*!
      example of request:
      POST http://server:port/ngrest/examples/users/create
      -- body -----------------------
      {
        "name": "John Doe",
        "email": "john.doe@example.com"
      }
      -------------------------------

      returns id of created user:
      -- response -------------------
      {
        "result": 1
      }
      -------------------------------
    */
    // *method: POST
    // *location: /create
    int create(const User& user);

    //! create get object by identifier
    /*!
      example of request:
      http://server:port/ngrest/examples/users/1
    */
    // *method: GET
    // *location: /{id}
    User get(int id) const;

    //! update whole user record
    /*!
      example of request:
      PUT http://server:port/ngrest/examples/users/1
      -- body -----------------------
      {
        "name": "James Doe",
        "email": "james.doe@example.com"
      }
    */
    // *method: PUT
    // *location: /{id}
    void update(int id, const User& user);

    //! update particular fields of specified user
    /*!
      example of request:
      PATCH http://server:port/ngrest/examples/users/1
      -- body -----------------------
      {
        "name": "Jimm Doe"
      }
    */
    // *method: PATCH
    // *location: /{id}
    void patch(int id, const UserPatch& user);

    //! delete existing user by identifier
    /*!
      example of request:
      DELETE http://server:port/ngrest/examples/users/1
    */
    // *method: DELETE
    // *location: /{id}
    void del(int id);
};

}
}

#endif
