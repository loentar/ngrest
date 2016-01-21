#ifndef NGREST_SERVICEDESCRIPTION_H
#define NGREST_SERVICEDESCRIPTION_H

#include <string>
#include <vector>

namespace ngrest {

struct OperationDescription
{
    std::string name;                              // service operation
    std::string location;                          // by default = name
                                                   // can be "add?a={a}&b={b}" or "get/{id}" or "put"
    int method;                                    // method depending on transport
    std::string methodStr;                         // method depending on transport in string form
    bool asynchronous;                             // is operation asynchronous
};


struct ServiceDescription
{
    std::string name;                              // service name
    std::string location;                          // by default = name
    std::vector<OperationDescription> operations;  // service operations
};

} // namespace ngrest

#endif // NGREST_SERVICEDESCRIPTION_H
