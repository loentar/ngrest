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

#include <string.h>

#include <string>
#include <unordered_map>

#include <core/utils/Exception.h>
#include <core/utils/Log.h>
#include <core/utils/stringutils.h>
#include <ngrest/common/Service.h>
#include <ngrest/common/Message.h>
#include <ngrest/common/ObjectModel.h>
#include <ngrest/json/JsonReader.h>
#include <ngrest/json/JsonWriter.h>

#include "ServiceDescription.h"
#include "ServiceWrapper.h"
#include "Phase.h"
#include "Engine.h"
#include "Transport.h"
#include "ServiceDispatcher.h"

namespace ngrest {

inline char fromHexChar(char hex)
{
    if (hex >= '0' && hex <= '9')
        return hex - '0';
    if (hex >= 'a' && hex <= 'f')
        return hex - 'a' + 0x0a;
    if (hex >= 'A' && hex <= 'F')
        return hex - 'A' + 0x0a;
    return -1;
}

char* urldecode(char* url)
{
    bool write = false;
    char high;
    char low;
    char* dst = url;
    for (; *url; ++url, ++dst) {
        if (*url == '%' && url[1] && url[2]) {
            high = fromHexChar(url[1]);
            low = fromHexChar(url[2]);
            if (high != -1 && low != -1) {
                *dst = high << 4 | low;
                url += 2;
                write = true;
                continue;
            }
        } else if (*url == '+') {
            *dst = ' ';
        } else if (write) {
            *dst = *url;
        }
    }
    if (write)
        *dst = '\0';

    return dst;
}

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

struct ResourcePath
{
    DeployedService* service = nullptr;
    ResourcePath* parent = nullptr;
    std::string name;
    std::unordered_map<std::string, ResourcePath*> children;
};

struct ServiceDispatcher::Impl
{
    std::unordered_map<std::string, DeployedService> deployedServices;
    ResourcePath root;

    std::string getServiceLocation(const ServiceDescription* serviceDescr)
    {
        if (serviceDescr->location.empty()) {
            std::string serviceLocation = serviceDescr->name;
            stringReplace(serviceLocation, ".", "/", true);
            return serviceLocation;
        } else {
            // validate service location
            NGREST_ASSERT(serviceDescr->location.find_first_of("{} \n\r\t?%&=\'\"") == std::string::npos,
                          "Service location is not valid");
            return serviceDescr->location;
        }
    }

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
        if (res.parameters.empty())
            baseLocation = location;
    }

    ResourcePath& getResourcePath(const std::string& path, bool create)
    {
        ResourcePath* curr = &root;
        std::string::size_type start = 0;
        std::string::size_type end = 0;
        while (end != std::string::npos) {
            end = path.find('/', start);
            const std::string& part = (end != std::string::npos)
                    ? path.substr(start, end - start) : path.substr(start);

            if (create) {
                ResourcePath*& next = curr->children[part];
                if (!next) {
                    next = new ResourcePath();
                    next->name = part;
                    next->parent = curr;
                }
                curr = next;
            } else {
                auto it = curr->children.find(part);
                NGREST_ASSERT(it != curr->children.end(), "Failed to get resource by path: " + path);
                curr = it->second;
                NGREST_ASSERT_NULL(curr);
            }

            start = end + 1;
        }

        return *curr;
    }

    void deleteResourcePath(ResourcePath* res) {
        for (;;) {
            ResourcePath* parent = res->parent;
            if (!parent || parent->service || !res->children.empty())
                break;
            auto it = parent->children.find(res->name);
            NGREST_ASSERT(it != parent->children.end(), "Failed to get resource by path");
            NGREST_ASSERT(it->second == res, "Internal exception!"); // should never happen
            delete res;
            parent->children.erase(it);
            res = parent;
        }
    }

    DeployedService* findServiceByPath(const std::string& path, std::string::size_type& matchedPos)
    {
        NGREST_ASSERT_PARAM(!path.empty() && path[0] == '/'); // should never happen

        ResourcePath* curr = &root;
        std::string::size_type start = 1;
        std::string::size_type end = 0;
        DeployedService* service = nullptr;

        const std::string::size_type pathSize = path.size();

        while (end != pathSize) {
            end = path.find('/', start);
            if (end == std::string::npos)
                end = pathSize;
            const std::string& part = path.substr(start, end - start);

            auto it = curr->children.find(part);
            if (it == curr->children.end()) {
                // no next path part
                break;
            }
            curr = it->second;
            NGREST_ASSERT_NULL(curr);
            if (curr->service) {
                service = curr->service;
                matchedPos = (end == pathSize) ? pathSize : (end + 1);
            }

            start = end + 1;
        }

        return service;
    }

};


ServiceDispatcher::ServiceDispatcher():
    impl(new Impl())
{
}

ServiceDispatcher::~ServiceDispatcher()
{
    delete impl;
}

void ServiceDispatcher::registerService(ServiceWrapper* wrapper)
{
    NGREST_ASSERT_PARAM(wrapper);

    const ServiceDescription* serviceDescr = wrapper->getDescription();
    NGREST_ASSERT(serviceDescr, "Service description is not set");

    const std::string& serviceName = serviceDescr->name;
    NGREST_ASSERT(!serviceName.empty(), "Service name cannot be empty");

    LogDebug() << "Registering service " << serviceName;

    // use default service location in case of service location is not set
    const std::string& serviceLocation = impl->getServiceLocation(serviceDescr);

    // test if service already registered
    DeployedService& deployedService = impl->deployedServices[serviceName];
    NGREST_ASSERT(!deployedService.wrapper, "Service " + serviceName + " is already registered");

    ResourcePath& serviceResPath = impl->getResourcePath(serviceLocation, true);
    NGREST_ASSERT(!serviceResPath.service, "Resource path " + serviceLocation
                  + " is already occupied by the service "
                  + serviceResPath.service->wrapper->getDescription()->name);
    serviceResPath.service = &deployedService;

    // parse operations locations
    for (const OperationDescription& operationDescr : serviceDescr->operations) {

        NGREST_ASSERT(!operationDescr.name.empty(), "Operation name cannot be empty. " + serviceName);

        std::string operationLocation = operationDescr.location.empty()
                ? operationDescr.name : operationDescr.location;

        NGREST_ASSERT_PARAM(!operationLocation.empty()); // should never happen
        if (operationLocation[0] == '/') // use '/' as location to access resource root
            operationLocation.erase(0, 1);

        // validate operation location
        NGREST_ASSERT(operationLocation.find_first_of(" \n\r\t") == std::string::npos,
                      "Operation location is not valid: " + serviceName + ": " + operationLocation);

        LogVerbose() << "Registering resource: " << operationDescr.methodStr
                     << " /" << serviceLocation << "/" << operationLocation;

        Resource resource;
        std::string baseLocation; // can be empty
        impl->parseResource(operationLocation, resource, baseLocation);
        resource.operation = &operationDescr; // it's ok, because ServiceDescription stored statically

        // check for path collision in resources
        auto itExisting = deployedService.paramResources.find(baseLocation);
        NGREST_ASSERT(itExisting == deployedService.paramResources.end() ||
                      itExisting->second.operation->method != operationDescr.method ||
                      itExisting->second.operation->parameters.size() != operationDescr.parameters.size(),
                      "Parametrized path [" + baseLocation + "] is already taken by "
                      + serviceName + "/" + itExisting->second.operation->name);

        itExisting = deployedService.staticResources.find(baseLocation);
        NGREST_ASSERT(itExisting == deployedService.staticResources.end() ||
                      itExisting->second.operation->method != operationDescr.method ||
                      itExisting->second.operation->parameters.size() != operationDescr.parameters.size(),
                      "Static path [" + baseLocation + "] is already taken by "
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

void ServiceDispatcher::unregisterService(ServiceWrapper* wrapper)
{
    NGREST_ASSERT_PARAM(wrapper);

    const ServiceDescription* serviceDescr = wrapper->getDescription();
    NGREST_ASSERT(serviceDescr, "Service description is not set");

    const std::string& serviceName = serviceDescr->name;
    NGREST_ASSERT(!serviceName.empty(), "Service name cannot be empty");

    LogDebug() << "Unregistering service " << serviceName;

    // use default service location in case of service location is not set
    const std::string& serviceLocation = impl->getServiceLocation(serviceDescr);

    // recursively free resource path
    ResourcePath& resourcePath = impl->getResourcePath(serviceLocation, false);
    resourcePath.service = nullptr;
    impl->deleteResourcePath(&resourcePath);

    // unregister deployed service
    auto count = impl->deployedServices.erase(serviceName);
    NGREST_ASSERT(count, "Service " + wrapper->getDescription()->name + " is not registered");

    LogDebug() << "Service " << serviceName << " has been unregistered";
}

void ServiceDispatcher::dispatchMessage(MessageContext* context)
{
    NGREST_ASSERT_NULL(context->request->path);
    const std::string& path = context->request->path;

    LogDebug() << "Dispatching message " << path;

    std::string::size_type begin = std::string::npos;
    DeployedService* service = impl->findServiceByPath(path, begin);
    NGREST_ASSERT(service, "No service found to handle resource " + path);

    const std::string& opLocation = path.substr(begin);
    NGREST_ASSERT(opLocation.find_first_of(" \n\r\t") == std::string::npos,
                  "Operation location is not valid: " + opLocation);

    int method = context->transport->getRequestMethod(context->request);

    // first look path in static resources
    auto itOpLocation = service->staticResources.equal_range(opLocation);
    for (auto it = itOpLocation.first; it != itOpLocation.second; ++it) {
        const Resource& resource = it->second;
        if (resource.operation->method == method) {
            // found it!
            if (context->engine)
                context->engine->runPhase(Phase::PreInvoke, context);
            LogDebug() << "Invoking service operation " << service->wrapper->getDescription()->name
                       << "/" << resource.operation->name;
            service->wrapper->invoke(resource.operation, context);
            return;
        }
    }


    // search for suitable parametrized resource
    Resource* resource = nullptr;
    std::string::size_type matchLength = 0;

    for (auto& paramResource : service->paramResources) {
        const std::string& basePath = paramResource.first;
        const std::string::size_type basePathSize = basePath.size();
        // '=' is required in case of base path is empty
        if (!path.compare(begin, basePathSize, basePath) && (matchLength <= basePathSize)) {
            Resource& currResource = paramResource.second;
            if (currResource.operation->method == method) {
                matchLength = basePathSize;
                resource = &currResource;
            }
        }
    }

    NGREST_ASSERT(resource, "Resource not found for path: " + path);

    // generate OM from request

    // path = "/calc/add?a={a}&b={b}"
    // path = "/calc/add?a=1111111&b=2"
    //                     ^ matchLength


    begin += matchLength;

    const char* pathCStr = path.c_str();
    Object* requestNode;
    NamedNode* lastNamedNode = nullptr;
    std::string::size_type end;
    std::string::size_type dividerSize;

    if (context->request->node) {
        // request have both of body and query
        // pointing lastNamedNode to last child of root query
        NGREST_ASSERT(context->request->node->type == NodeType::Object, "Body must be an Object");
        requestNode = static_cast<Object*>(context->request->node);
        lastNamedNode = static_cast<NamedNode*>(requestNode->firstChild);
        if (lastNamedNode) {
            while (lastNamedNode->nextSibling)
                lastNamedNode = lastNamedNode->nextSibling;
        }
    } else {
        // create empty object to serialize query to it
        requestNode = context->pool->alloc<Object>();
        context->request->node = requestNode;
    }

    for (int i = 0, l = resource->parameters.size(); i < l; ++i) {
        const Parameter& parameter = resource->parameters[i];

        if ((i + 1) < l) {
            const Parameter& nextParameter = resource->parameters[i + 1];
            const std::string& nextDivider = nextParameter.divider;
            end = path.find(nextDivider, begin);
            NGREST_ASSERT(end != std::string::npos,
                          "Can't find divider [" + nextDivider + "] for path: " + path);
            dividerSize = nextDivider.size();
        } else {
            end = path.size();
            dividerSize = 0;
        }

        const char* name = context->pool->putCString(parameter.name.c_str(), true);
        char* value = context->pool->putCString(pathCStr + begin, end - begin, true);
        char* valueEnd = urldecode(value);

        NamedNode* namedNode = context->pool->alloc<NamedNode>(name);

        if (lastNamedNode) {
            lastNamedNode->nextSibling = namedNode;
        } else {
            requestNode->firstChild = namedNode;
        }
        lastNamedNode = namedNode;

        // detect type of value
        if (!strcmp(value, "null")) {
            // namedNode->node = nullptr; // already nullptr
        } else if (*value == '[' || *value == '{') {
            // array or object
            namedNode->node = json::JsonReader::read(value, context->pool);
        } else {
            if (*value == '"') {
                // quoted string/enum.
                ++value;
                *(--valueEnd) = '\0';
            }
            namedNode->node = context->pool->alloc<Value>(ValueType::String, value);
        }

        begin = end + dividerSize;
    }


#ifdef DEBUG
    json::JsonWriter::write(requestNode, context->response->poolBody);
    LogDebug() << "Generated request:\n---------------------\n"
               << context->response->poolBody->flatten()->buffer
               << "\n---------------------\n";
    context->response->poolBody->reset();
#endif

    if (context->engine)
        context->engine->runPhase(Phase::PreInvoke, context);

    LogDebug() << "Invoking service operation " << service->wrapper->getDescription()->name
               << "/" << resource->operation->name;
    service->wrapper->invoke(resource->operation, context);
}

std::vector<ServiceWrapper*> ServiceDispatcher::getServices() const
{
    std::vector<ServiceWrapper*> services;
    services.reserve(impl->deployedServices.size());
    for (const auto& service : impl->deployedServices)
        services.push_back(service.second.wrapper);
    return services;
}

ServiceWrapper* ServiceDispatcher::getService(const std::string& name) const
{
    auto it = impl->deployedServices.find(name);
    return (it == impl->deployedServices.end()) ? nullptr : it->second.wrapper;
}

} // namespace ngrest

