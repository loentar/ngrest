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

#include <unordered_map>

#include <core/utils/Log.h>
#include <core/utils/stringutils.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/HttpException.h>
#include <ngrest/engine/Transport.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/ServiceDispatcher.h>
#include <ngrest/engine/ServiceWrapper.h>
#include <ngrest/engine/ServiceDescription.h>
#include <ngrest/engine/Phase.h>
#include <ngrest/engine/Filter.h>
#include <ngrest/engine/FilterDispatcher.h>

#include "ServerStatus.h"

namespace ngrest {

typedef std::unordered_map<std::string, std::string> ParamsMap;

static const char* css = R"(
html {
  font-family:sans-serif;
  font-size: 14px;
  color: #222;
}
a {
  color: #18d;
  text-decoration: none;
}
a:hover, a:active {
  color: #07c;
  text-decoration: underline;
}
label {
  font-weight: bold;
}
.error {
  color: red;
}
.success {
  color: darkgreen;
}
.nocontent {
  color: lightgray;
}
)";

static const char* jsUtil = R"(
    function $(name) {
      return document.querySelectorAll(name);
    }

    function ajax(method, url, body, headers) {

      var asyncWrapper = {
        success: function(fn) {
          this.onSuccess = fn;
          return this;
        },
        error: function(fn) {
          this.onError = fn;
          return this;
        },
        done: function(fn) {
          this.onDone = fn;
          return this;
        }
      };

      var request = new XMLHttpRequest();
      request.open(method, url, true);
      request.onreadystatechange = function() {
        if (request.readyState == 4) {
          if (request.status == 200) {
            if (asyncWrapper.onSuccess) {
              asyncWrapper.onSuccess(request.responseText);
            }
          } else {
              if (asyncWrapper.onError) {
                asyncWrapper.onError(request.responseText, request.status);
              }
          }
          if (asyncWrapper.onDone)
            asyncWrapper.onDone(request.responseText);
        }
      };
      for (var name in headers)
        request.setRequestHeader(name, headers[name]);
      request.send(body);

      return asyncWrapper;
    };


    function nullableClicked(name) {
        $('#' + name)[0].disabled = $('#check_' + name)[0].checked ? '' : 'disabled';
    }

)";

class TemplateProcessor
{
public:
    static void process(std::string& templ, const ParamsMap& parameters)
    {
        std::string::size_type begin = 0;
        std::string::size_type end = 0;

        for (;;) {
            begin = templ.find("{{", end);
            if (begin == std::string::npos)
                break;
            end = templ.find("}", begin + 2);
            if (end == std::string::npos)
                break;

            const std::string& parameter = templ.substr(begin + 2, end - begin - 2);
            auto itParameter = parameters.find(parameter);
            std::string value;
            if (itParameter == parameters.end()) {
                LogWarning() << "Parameter {{" + parameter + "}} is not found in parameters list!";
            } else {
                value = itParameter->second;
            }
            templ.replace(begin, end - begin + 2, value);
            end = begin + value.size();
        }
    }
};

std::string getServiceLocation(const ServiceDescription* serviceDescr)
{
    if (serviceDescr->location.empty()) {
        std::string serviceLocation = serviceDescr->name;
        stringReplace(serviceLocation, ".", "/", true);
        return serviceLocation;
    } else {
        return serviceDescr->location;
    }
}

const char* paramTypeToString(ParameterDescription::Type type)
{
    switch (type) {
    case ParameterDescription::Type::Undefined:
        return "Undefined";

    case ParameterDescription::Type::String:
        return "String";

    case ParameterDescription::Type::Number:
        return "Number";

    case ParameterDescription::Type::Boolean:
        return "Boolean";

    case ParameterDescription::Type::Array:
        return "Array";

    case ParameterDescription::Type::Object:
        return "Object";

    default:
        return "UNKNOWN";
    }
}

void ServerStatus::getFilters(MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    Header* headerContentType = context.pool->alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    MemPool* pool = context.response->poolBody;

    pool->putCString("<html><head>"
                    "<title>Deployed filters - ngrest</title>"
                    "<style>");
    pool->putCString(css);
    pool->putCString("</style></head><body>"
                     "<h1>ngrest</h1>&nbsp;<a href='/ngrest/services'>services</a>"
                     "&nbsp;&nbsp;<a href='/ngrest/filters'>filters</a>"
                    "<h2>Deployed filters:</h2>");
    FilterDispatcher* filterDispatcher = context.engine->getFilterDispatcher();
    if (filterDispatcher) {
        for (int i = 0; i < static_cast<int>(Phase::Count); ++i) {
            Phase phase = static_cast<Phase>(i);
            const std::list<Filter*>& filters = filterDispatcher->getFilters(phase);
            pool->putCString("<p><li><h3>Phase ");
            pool->putCString(PhaseInfo::phaseToString(phase));
            pool->putCString(":</h3></li><ul>");
            if (!filters.empty()) {
                for (const Filter* filter : filters) {
                    pool->putCString("<li>");
                    pool->putCString(filter->getName().c_str());
                    const auto& dependencies = filter->getDependencies();
                    if (!dependencies.empty()) {
                        pool->putCString(" -&gt; ");
                        for (const std::string& dep : dependencies) {
                            pool->putCString(dep.c_str());
                            pool->putCString(" ");
                        }
                    }
                    pool->putCString("</li>");
                }
            } else {
                pool->putCString("<span class=\"nocontent\">No filters registered within this phase</span>");
            }
            pool->putCString("</ul>");
        }
    } else {
        pool->putCString("<span class=\"nocontent\">Filter dispatcher is inactive</span>");
    }

    pool->putCString("</body></html>");
}

void ServerStatus::getServices(MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    Header* headerContentType = context.pool->alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    MemPool* pool = context.response->poolBody;

    pool->putCString("<html><head>"
                    "<title>Deployed services - ngrest</title>"
                    "<style>");
    pool->putCString(css);
    pool->putCString("</style></head><body>"
                     "<h1>ngrest</h1>&nbsp;<a href='/ngrest/services'>services</a>"
                     "&nbsp;&nbsp;<a href='/ngrest/filters'>filters</a>"
                     "<h2>Deployed services:</h2>");
    const std::vector<ServiceWrapper*>& services = context.engine->getServiceDispatcher().getServices();
    for (const ServiceWrapper* service : services) {
        const ServiceDescription* serviceDescr = service->getDescription();
        pool->putCString("<p><h3>");
        pool->putCString(("<a href=\"/ngrest/service/" + serviceDescr->name + "\">"
                         + serviceDescr->name + "</a>").c_str());
        pool->putCString("</h3><ul>");
        for (const OperationDescription& opDescr : serviceDescr->operations) {
            pool->putCString(("<li><a href=\"/ngrest/operation/" + serviceDescr->name + "/"
                             + opDescr.name + "\">" + opDescr.name + "</a></li>").c_str());
        }
        pool->putCString("</ul></p>");
    }

    pool->putCString("</body></html>");
}

void ServerStatus::getService(const std::string& name, MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    const ServiceWrapper* service = context.engine->getServiceDispatcher().getService(name);
    NGREST_ASSERT_HTTP(service, HTTP_STATUS_404_NOT_FOUND, "Service not found: " + name);

    Header* headerContentType = context.pool->alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    MemPool* pool = context.response->poolBody;

    const ServiceDescription* descr = service->getDescription();

    pool->putCString("<html><head><title>");
    pool->putCString(name.c_str());
    pool->putCString(" - ngrest</title><style>");
    pool->putCString(css);
    pool->putCString("</style></head><body>"
                    "<h1><a href='/ngrest/services'>ngrest</a><h1>"
                    "<h2>");
    pool->putCString(("<a href=\"/ngrest/service/" + descr->name + "\">"
                    + descr->name + "</a>").c_str());
    pool->putCString("</h2><p><h3>");
    pool->putCString(descr->description.c_str());
    pool->putCString("</h3><pre>");
    pool->putCString(descr->details.c_str());
    pool->putCString("</pre></p><ul>");
    for (const OperationDescription& opDescr : descr->operations) {
        pool->putCString(("<li><hr/><p><a href=\"/ngrest/operation/" + descr->name + "/"
                         + opDescr.name + "\">" + opDescr.name + "</a>").c_str());
        pool->putCString("<h4>");
        pool->putCString(opDescr.description.c_str());
        pool->putCString("</h4><small><pre>");
        pool->putCString(opDescr.details.c_str());
        pool->putCString("</pre></small></p></li>");
    }
    pool->putCString("</ul>");

    pool->putCString("</body></html>");
}

void ServerStatus::getOperation(const std::string& serviceName, const std::string& operationName,
                                MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    const ServiceWrapper* service = context.engine->getServiceDispatcher().getService(serviceName);
    NGREST_ASSERT_HTTP(service, HTTP_STATUS_404_NOT_FOUND, "Service not found: " + serviceName);

    const ServiceDescription* serviceDescr = service->getDescription();
    const OperationDescription* opDescr = nullptr;

    for (const OperationDescription& operation : serviceDescr->operations) {
        if (operation.name == operationName) {
            opDescr = &operation;
            break;
        }
    }

    NGREST_ASSERT_HTTP(opDescr, HTTP_STATUS_404_NOT_FOUND, "Service operation not found: "
                       + serviceName + "/" + operationName);


    Header* headerContentType = context.pool->alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    const std::string& serviceLocation = getServiceLocation(serviceDescr);

    MemPool* pool = context.response->poolBody;

    std::string templ = R"(
<html>
<head>
  <title>{{service}}/{{operation}} - ngrest</title>
  <style>{{css}}</style>
</head>
<body>
  <h1><a href='/ngrest/services'>ngrest</a><h1>
  <h2>{{serviceHref}} / {{operationHref}}</h2>
  <p>
  <h3>{{serviceDescr}}</h3>
  <pre>{{serviceDetails}}</pre>
  <h4>{{operationDescr}}</h4>
  <small><pre>{{operationDetails}}</pre></small>
  <span><label>Method: </label>{{method}}</span><br/>
  <span><label>Location: </label><a href="{{location}}">{{location}}</a></span><br/>
  <span><label>Asynchronous: </label>{{asynchronous}}</span><br/>
  </p>
  {{form}}
  <script language='javascript'>
    var method = '{{method}}';
    var resLocation = '{{location}}';
    {{jsutil}}
    {{jsform}}
  </script>
</body>
</html>
)";

    static const char* jsFrom = R"(
    var form = $('form')[0];
    var result = $('#result')[0];

    function parseField(input, inputValue) {
      if (input.className === 'any') {
        try {
          return JSON.parse(inputValue);
        } catch (e) {
          return inputValue;
        }
      } else if (input.className === 'object') {
        try {
          var value = JSON.parse(inputValue);
          if (!typeof value === 'object' || (value instanceof Array)) {
            alert("parameter " + input.name + " must be object")
            return;
          }
          return value;
        } catch (e) {
          alert("Error reading array parameter " + input.name + ": " + e.toString()
            + "\nmust be JSON, e.g. {...}")
        }
      } else if (input.className === 'array') {
        try {
          var value = JSON.parse(input.value);
          if (!(value instanceof Array)) {
            alert("parameter " + input.name + " must be array")
            return;
          }
          return value;
        } catch (e) {
          alert("Error reading array parameter " + input.name + ": " + e.toString()
            + "\nmust be JSON Array, e.g. [...]")
        }
      } else {
        return inputValue;
      }
    }

    form.onsubmit = function(e) {
      e.preventDefault();

      var url = resLocation;
      var inputs = $('form *[name]');
      var bodyFields;
      var headers;
      for (var i = 0, l = inputs.length; i < l; ++i) {
        var input = inputs[i];
        var value = input.value;
        var parsedValue = null;
        var check = $('#check_' + input.id);

        if (check.length && !check[0].checked) {
          value = null;
        } else {
          parsedValue = parseField(input, value);
          if (parsedValue === undefined)
            return;
        }

        var param = '{' + input.name + '}';
        if (url.indexOf(param) != -1) {
          url = url.replace(param, value);
        } else {
          if (!bodyFields) {
            bodyFields = {};
            headers = {'Content-Type': 'application/json'};
          }
          bodyFields[input.name] = parsedValue;
        }
      }

      ajax(method, url, bodyFields && JSON.stringify(bodyFields), headers)
        .success(function(res){
          result.innerHTML = res ? '<h3>Success:</h3><pre class="success">' + res + '</pre>'
                                : '<h3>Success</h3><span class="nocontent">NO CONTENT</span>'
        })
        .error(function(res, status){
          result.innerHTML = '<h3>Error #' + status + ':</h3><pre class="error">' + res + '</pre>';
        });
    }
)";


    std::string form;

    std::string location = opDescr->location.empty() ? opDescr->name : opDescr->location;
    NGREST_ASSERT_PARAM(!location.empty()); // should never happen
    if (location[0] == '/')
        location.erase(0, 1);

    form = "<hr/><h2>Test operation</h2><p>";
    form += "<form action='#'>";
    form += "<table><tbody>";

    for (const ParameterDescription& param : opDescr->parameters) {
        const std::string& parameter = param.name;

        form += "<tr>";
        form += "<td>";
        form += "<label for='" + parameter + "'>" + parameter + "</label>";
        if (param.nullable)
            form += "<input type='checkbox' id='check_" + parameter + "' checked='checked' onclick='nullableClicked(\""
                    + parameter + "\");'></input>";
        form += "</td>";
        if (param.type == ParameterDescription::Type::Object) {
            form += "<td><textarea cols='50' rows='3' id='" + parameter + "' name='" + parameter + "' placeholder='"
                      + paramTypeToString(param.type) + "' class='object'></textarea></td>";
        } else if (param.type == ParameterDescription::Type::Array) {
            form += "<td><textarea cols='50' rows='3' id='" + parameter + "' name='" + parameter + "' placeholder='"
                    + paramTypeToString(param.type) + "' class='array'></textarea></td>";
        } else if (param.type == ParameterDescription::Type::Any) {
            form += "<td><textarea cols='50' rows='3' id='" + parameter + "' name='" + parameter + "' placeholder='"
                    + paramTypeToString(param.type) + "' class='any'></textarea></td>";
        } else {
            form += "<td><input type='text' id='" + parameter + "' name='" + parameter + "' placeholder='"
                      + paramTypeToString(param.type) + "'></input></td>";
        }
        form += "</tr>";
    }
    form += "</tbody></table>";
    form += "</p>";
    form += "<input type='submit'></input>";
    form += "</form>";
    form += "<hr/>";
    form += "<div id='result'></div>";

    TemplateProcessor::process(templ, ParamsMap {
        {"css", css},
        {"jsutil", jsUtil},
        {"service", serviceDescr->name},
        {"serviceDescr", serviceDescr->description},
        {"serviceDetails", serviceDescr->details},
        {"operation", opDescr->name},
        {"operationDescr", opDescr->description},
        {"operationDetails", opDescr->details},
        {"serviceHref", "<a href=\"/ngrest/service/" + serviceDescr->name + "\">"
                        + serviceDescr->name + "</a>"},
        {"operationHref", "<a href=\"/ngrest/operation/" + serviceDescr->name + "/"
                          + opDescr->name + "\">" + opDescr->name + "</a>"},
        {"method", opDescr->methodStr},
        {"location", "/" + serviceLocation + "/" + location},
        {"locationHref", "<a href='/" + serviceLocation + "/" + location
         + "'>" + opDescr->location + "</a>"},
        {"asynchronous", opDescr->asynchronous ? "true" : "false"},
        {"form", form},
        {"jsform", jsFrom},
    });

    pool->putCString(templ.c_str());
}

}
