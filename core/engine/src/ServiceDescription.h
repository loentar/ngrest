#ifndef NGREST_SERVICEDESCRIPTION_H
#define NGREST_SERVICEDESCRIPTION_H

#include <string>
#include <vector>

namespace ngrest {

//! operation parameter description
struct ParameterDescription
{
    //! type of operation parameter
    enum class Type
    {
        Unknown,
        Undefined,
        String,
        Number,
        Boolean,
        Array,
        Object
    };

    std::string name;  // parameter name
    Type type;         // type of parameter
};

//! operation description
struct OperationDescription
{
    std::string name;                              // service operation
    std::string location;                          // by default = name
                                                   // can be "add?a={a}&b={b}" or "get/{id}" or "put"
    int method;                                    // method depending on transport
    std::string methodStr;                         // method depending on transport in string form
    bool asynchronous;                             // is operation asynchronous
    std::string description;                       // text description of the operation
    std::string details;                           // text details of the operation
    std::vector<ParameterDescription> parameters;  // parameters
    ParameterDescription::Type result;             // type of result value
};

//! service description
struct ServiceDescription
{
    std::string name;                              // service name
    std::string location;                          // by default = name
    std::string description;                       // text description of the service
    std::string details;                           // text details of the service
    std::vector<OperationDescription> operations;  // service operations
};

} // namespace ngrest

#endif // NGREST_SERVICEDESCRIPTION_H
