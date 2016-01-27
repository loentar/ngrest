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
