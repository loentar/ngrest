#include <string>
#include <unordered_map>

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/Log.h>
#include <ngrest/common/Service.h>
#include <ngrest/common/Message.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/json/JsonWriter.h>

#include "ServiceDescription.h"
#include "ServiceWrapper.h"
#include "Transport.h"
#include "Deployment.h"

namespace ngrest {

struct Parameter
{
    std::string name;
    std::string divider;
};

struct Resource
{
    std::vector<Parameter> parameters;
    const OperationDescription* operation = nullptr;
};

struct DeployedService
{
    ServiceWrapper* wrapper = nullptr;
    std::unordered_multimap<std::string, Resource> staticResources;
    std::unordered_multimap<std::string, Resource> paramResources;
};

struct Deployment::Impl
{
    std::unordered_map<std::string, DeployedService> deployedServices;

    // location may be "add?a={a}&b={b}" or "get/{id}" or "echo"
    void parseResource(const std::string& location, Resource& res, std::string& baseLocation)
    {
        std::string::size_type begin = 0;
        std::string::size_type end = 0;
        Parameter parameter;
        for (;;) {
            begin = location.find('{', end);
            if (begin == std::string::npos)
                break;

            if (end)
                parameter.divider = location.substr(end, begin - end);

            end = location.find('}', begin + 1);
            NGREST_ASSERT(end != std::string::npos,
                          "'}' expected while parsing resource parameter: " + location);
            parameter.name = location.substr(begin + 1, end - begin - 1);

            if (res.parameters.empty())
                baseLocation = location.substr(0, begin); // "add?a="  or "get/"
            res.parameters.push_back(parameter);
            ++end;
        }
        if (baseLocation.empty())
            baseLocation = location;
    }
};


Deployment::Deployment():
    impl(new Impl())
{
}

Deployment::~Deployment()
{
    delete impl;
}

void Deployment::registerService(ServiceWrapper* wrapper)
{
    NGREST_ASSERT_PARAM(wrapper);

    const ServiceDescription* serviceDescr = wrapper->description();
    NGREST_ASSERT(serviceDescr, "Service description is not set");

    const std::string& serviceName = serviceDescr->name;
    NGREST_ASSERT(!serviceName.empty(), "Service name cannot be empty");

    LogDebug() << "Registering service " << serviceName;

    // use service name as location in case of service location is not set
    const std::string& serviceLocation = serviceDescr->location.empty()
            ? serviceName : serviceDescr->location;
    // validate service location
    NGREST_ASSERT(serviceLocation.find_first_of("{} \n\r\t?%&=\'\"") == std::string::npos,
                  "Service location is not valid");

    // test if service already registered
    DeployedService& deployedService = impl->deployedServices[serviceLocation];
    NGREST_ASSERT(!deployedService.wrapper, "Service " + serviceName + " is already registered");

    // parse operations locations
    for (auto it = serviceDescr->operations.begin(); it != serviceDescr->operations.end(); ++it) {
        const OperationDescription& operationDescr = *it;

        NGREST_ASSERT(!operationDescr.name.empty(), "Operation name cannot be empty. " + serviceName);

        const std::string& operationLocation = operationDescr.location.empty()
                ? operationDescr.name : operationDescr.location;

        // validate operation location
        NGREST_ASSERT(operationLocation.find_first_of(" \n\r\t") == std::string::npos,
                      "Operation location is not valid: " + serviceName + ": " + operationLocation);

        LogDebug() << "Registering resource: " <<serviceName << " : "
                   << "/" << serviceLocation << "/" << operationLocation;

        Resource resource;
        std::string baseLocation;
        impl->parseResource(operationLocation, resource, baseLocation);
        resource.operation = &operationDescr; // it's ok, because ServiceDescription stored statically
        NGREST_ASSERT(!baseLocation.empty(), "BaseLocation is empty!"); // should never happen

        // check for path collision in resources
        auto itExisting = deployedService.paramResources.find(baseLocation);
        NGREST_ASSERT(itExisting == deployedService.paramResources.end() ||
                      itExisting->second.operation->method != operationDescr.method,
                      "Static path " + baseLocation + " is already taken by "
                      + serviceName + "/" + itExisting->second.operation->name);

        itExisting = deployedService.staticResources.find(baseLocation);
        NGREST_ASSERT(itExisting == deployedService.staticResources.end() ||
                      itExisting->second.operation->method != operationDescr.method,
                      "Static path " + baseLocation + " is already taken by "
                      + serviceName + "/" + itExisting->second.operation->name);


        if (resource.parameters.empty()) {
            // static resource which don't have parameters
            deployedService.staticResources.insert({{baseLocation, resource}});
        } else {
            // parametrized resource
            deployedService.paramResources.insert({{baseLocation, resource}});
        }
    }

    deployedService.wrapper = wrapper;
    LogDebug() << "Service " << serviceName << " has been registered";
}

void Deployment::unregisterService(ServiceWrapper* wrapper)
{
    NGREST_ASSERT_PARAM(wrapper);

    const ServiceDescription* serviceDescr = wrapper->description();
    NGREST_ASSERT(serviceDescr, "Service description is not set");

    const std::string& serviceName = serviceDescr->name;
    NGREST_ASSERT(!serviceName.empty(), "Service name cannot be empty");

    LogDebug() << "Unregistering service " << serviceName;

    // use service name as location in case of service location is not set
    const std::string& serviceLocation = serviceDescr->location.empty() ?
                serviceName : serviceDescr->location;
    // validate service location
    NGREST_ASSERT(serviceLocation.find_first_of("{} \n\r\t?%&=\'\"") == std::string::npos,
                  "Service location is not valid");

    auto count = impl->deployedServices.erase(serviceLocation);
    NGREST_ASSERT(count, "Service " + wrapper->description()->name + " is not registered");

    LogDebug() << "Service " << serviceName << " has been unregistered";
}

void Deployment::dispatchMessage(MessageContext* context)
{
    const std::string& path = context->request->path;

    LogDebug() << "Dispatching message " << path;

    // parse service location
    std::string::size_type begin = path.find_first_not_of('/');
    NGREST_ASSERT(begin != std::string::npos, "Canot get start of service location in path: " + path);
    std::string::size_type end = path.find('/', begin);
    NGREST_ASSERT(end != std::string::npos, "Canot get end of service location in path: " + path);

    // validate service location
    const std::string& serviceLocation = path.substr(begin, end - begin);
    NGREST_ASSERT(serviceLocation.empty() ||
                  serviceLocation.find_first_of("{} \n\r\t?%&=\'\"") == std::string::npos,
                  "Service location is not valid: " + serviceLocation);

    // parse operation location
    begin = path.find_first_not_of('/', end);
    NGREST_ASSERT(begin != std::string::npos, "Canot get start of service location in URL: " + path);
    end = path.find('/', begin);
    const std::string& opLocation = path.substr(begin, (end == std::string::npos) ? end : (end - begin));

    NGREST_ASSERT(opLocation.empty() || opLocation.find_first_of(" \n\r\t") == std::string::npos,
                  "Operation location is not valid: " + opLocation);


    auto itService = impl->deployedServices.find(serviceLocation);
    NGREST_ASSERT(itService != impl->deployedServices.end(),
                  "Can't find service by path: " + path);

    DeployedService& service = itService->second;

    int method = context->transport->getRequestMethod(context->request);

    // first look path in static resources
    auto itOpLocation = service.staticResources.equal_range(opLocation);
    for (auto it = itOpLocation.first; it != itOpLocation.second; ++it) {
        const Resource& resource = it->second;
        if (resource.operation->method == method) {
            // found it!
            service.wrapper->invoke(resource.operation, context);
            return;
        }
    }


    // search for suitable parametrized resource
    Resource* resource = nullptr;
    std::string::size_type matchLength = 0;

    for (auto it = service.paramResources.begin(); it != service.paramResources.end(); ++it) {
        const std::string& basePath = it->first;
        const std::string::size_type basePathSize = basePath.size();
        if (!path.compare(begin, basePathSize, basePath) && (matchLength < basePathSize)) {
            Resource& currResource = it->second;
            if (currResource.operation->method == method) {
                matchLength = basePathSize;
                resource = &currResource;
            }
        }
    }

    NGREST_ASSERT(resource, "Resource not found for path: " + path);
    NGREST_ASSERT(!context->request->node, "Request must have just one of query or body, but have both");

    // generate OM from request

    // path = "/calc/add?a={a}&b={b}"
    // path = "/calc/add?a=1111111&b=2"
    //                     ^ matchLength


    begin += matchLength;

    const char* pathCStr = path.c_str();
    Object* requestNode = context->pool.alloc<Object>();
    NamedNode* lastNamedNode = nullptr;

    for (int i = 0, l = resource->parameters.size(); i < l; ++i) {
        const Parameter& parameter = resource->parameters[i];

        if ((i + 1) < l) {
            const Parameter& nextParameter = resource->parameters[i];
            const std::string& nextDivider = nextParameter.divider;
            end = path.find(nextDivider, begin);
            NGREST_ASSERT(end != std::string::npos,
                          "Can't find divider [" + nextDivider + "] for path: " + path);
        } else {
            end = path.size();
        }

        const char* name = context->pool.putCString(parameter.name.c_str(), true);
        const char* value = context->pool.putCString(pathCStr + begin, end - begin, true);

        NamedNode* namedNode = context->pool.alloc<NamedNode>();
        namedNode->name = name;

        if (lastNamedNode) {
            lastNamedNode->nextSibling = namedNode;
        } else {
            requestNode->firstChild = namedNode;
        }
        lastNamedNode = namedNode;

        Value* valueNode = context->pool.alloc<Value>(ValueType::String);
        valueNode->value = value;
        namedNode->node = valueNode;
    }

//#ifdef DEBUG
    json::JsonWriter::write(requestNode, context->response->poolBody);
    LogDebug() << "Generated request:\n---------------------\n"
               << context->response->poolBody.flatten()->buffer
               << "\n---------------------\n";
    context->response->poolBody.reset();
//#endif

    context->request->node = requestNode;

    LogDebug() << "Invoking service operation " << service.wrapper->description()->name
               << "/" << resource->operation->name;
    service.wrapper->invoke(resource->operation, context);
}

} // namespace ngrest

