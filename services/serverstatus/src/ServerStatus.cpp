#include <unordered_map>

#include <ngrest/utils/Log.h>
#include <ngrest/utils/stringutils.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/HttpException.h>
#include <ngrest/engine/Transport.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/ServiceDispatcher.h>
#include <ngrest/engine/ServiceWrapper.h>
#include <ngrest/engine/ServiceDescription.h>

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
)";

static const char* jsUtil = R"(
    function $(name) {
      return document.querySelectorAll(name);
    }

    function ajax(method, url, body) {

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
      request.send(body);

      return asyncWrapper;
    };

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

void ServerStatus::getServices(MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    Header* headerContentType = context.pool.alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    MemPool& pool = context.response->poolBody;

    pool.putCString("<html><head>"
                    "<title>Deployed services - ngrest</title>"
                    "<style>");
    pool.putCString(css);
    pool.putCString("</style></head><body>"
                    "<h1><a href='/ngrest/services'>ngrest</a><h1>"
                    "<h2>Deployed services:</h2>");
    const std::vector<ServiceWrapper*>& services = context.engine->getDispatcher().getServices();
    for (auto itSvc = services.begin(), endService = services.end(); itSvc != endService; ++itSvc) {
        const ServiceWrapper* service = *itSvc;
        const ServiceDescription* descr = service->getDescription();
        pool.putCString("<p><h3>");
        pool.putCString(("<a href=\"/ngrest/service/" + descr->name + "\">" + descr->name + "</a>").c_str());
        pool.putCString("</h3><ul>");
        for (auto itOp = descr->operations.begin(), endOp = descr->operations.end(); itOp != endOp; ++itOp) {
            const OperationDescription* opDescr = &*itOp;
            pool.putCString(("<li><a href=\"/ngrest/operation/" + descr->name + "/"
                             + opDescr->name + "\">" + opDescr->name + "</a></li>").c_str());
        }
        pool.putCString("</ul></p>");
    }

    pool.putCString("</body></html>");

    context.callback->success();
}

void ServerStatus::getService(const std::string& name, MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    const ServiceWrapper* service = context.engine->getDispatcher().getService(name);
    NGREST_ASSERT_HTTP(service, HTTP_STATUS_404_NOT_FOUND, "Service not found: " + name);

    Header* headerContentType = context.pool.alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    MemPool& pool = context.response->poolBody;

    const ServiceDescription* descr = service->getDescription();

    pool.putCString("<html><head><title>");
    pool.putCString(name.c_str());
    pool.putCString(" - ngrest</title><style>");
    pool.putCString(css);
    pool.putCString("</style></head><body>"
                    "<h1><a href='/ngrest/services'>ngrest</a><h1>"
                    "<h2>");
    pool.putCString(("<a href=\"/ngrest/service/" + descr->name + "\">"
                    + descr->name + "</a>").c_str());
    pool.putCString("</h2><ul>");
    for (auto itOp = descr->operations.begin(), endOp = descr->operations.end(); itOp != endOp; ++itOp) {
        const OperationDescription* opDescr = &*itOp;
        pool.putCString(("<li><a href=\"/ngrest/operation/" + descr->name + "/"
                         + opDescr->name + "\">" + opDescr->name + "</a></li>").c_str());
    }
    pool.putCString("</ul>");

    pool.putCString("</body></html>");

    context.callback->success();
}

void ServerStatus::getOperation(const std::string& serviceName, const std::string& operationName,
                                MessageContext& context)
{
    NGREST_ASSERT_HTTP(context.transport->getType() == Transport::Type::Http,
                       HTTP_STATUS_501_NOT_IMPLEMENTED,
                       "This service only supports HTTP transport");

    HttpResponse* response = static_cast<HttpResponse*>(context.response);
    const ServiceWrapper* service = context.engine->getDispatcher().getService(serviceName);
    NGREST_ASSERT_HTTP(service, HTTP_STATUS_404_NOT_FOUND, "Service not found: " + serviceName);

    const ServiceDescription* serviceDescr = service->getDescription();
    const OperationDescription* opDescr = nullptr;

    for (auto itOp = serviceDescr->operations.begin(), endOp = serviceDescr->operations.end(); itOp != endOp; ++itOp) {
        if (itOp->name == operationName) {
            opDescr = &*itOp;
            break;
        }
    }

    NGREST_ASSERT_HTTP(opDescr, HTTP_STATUS_404_NOT_FOUND, "Service operation not found: "
                       + serviceName + "/" + operationName);


    Header* headerContentType = context.pool.alloc<Header>("Content-Type", "text/html");
    response->headers = headerContentType;

    const std::string& serviceLocation = getServiceLocation(serviceDescr);

    MemPool& pool = context.response->poolBody;

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
  <span><label>Method: </label>{{method}}</span><br/>
  <span><label>Location: </label>{{location}}</span><br/>
  <span><label>Asynchronous: </label>{{asynchronous}}</span><br/>
  </p>
  {{form}}
  <script language='javascript'>
    {{jsutil}}
    {{jsform}}
  </script>
</body>
</html>
)";

    static const char* jsFrom = R"(
    var form = $('form')[0];
    var result = $('#result')[0];

    form.onsubmit = function(e) {
      e.preventDefault();

      var action = form.action;
      var inputs = $('form *[name]');
      for (var i = 0, l = inputs.length; i < l; ++i) {
        var input = inputs[i];
        action = action.replace('{' + input.name + '}', input.value);
      }

      ajax(form.method, action)
        .success(function(res){
          result.innerHTML = '<h3>Success:</h3><pre class="success">' + res + '</pre>';
        })
        .error(function(res, status){
          result.innerHTML = '<h3>Error #' + status + ':</h3><pre class="error">' + res + '</pre>';
        });
    }
)";


    std::string form;

    if (static_cast<HttpMethod>(opDescr->method) == HttpMethod::GET) {
        const std::string& location = opDescr->location.empty() ? opDescr->name : opDescr->location;
        form = "<hr/><h2>Test operation</h2><p>";
        form += "<form action='/" + serviceLocation + "/" + location
                + "' method='" + opDescr->methodStr + "'>";
        std::string::size_type begin = 0;
        std::string::size_type end = 0;

        for (;;) {
            begin = location.find('{', end);
            if (begin == std::string::npos)
                break;
            end = location.find('}', begin + 1);
            if (end == std::string::npos)
                break;

            const std::string& parameter = location.substr(begin + 1, end - begin - 1);

            form += "<label for='" + parameter + "'>" + parameter + " = </label>";
            form += "<input type='text' id='" + parameter + "' name='" + parameter + "'></input>";
            form += "<br/>";

            end = begin + 1;
        }
        form += "</p>";
        form += "<input type='submit'></input>";
        form += "</form>";
        form += "<hr/>";
        form += "<div id='result'></div>";
    }

    TemplateProcessor::process(templ, ParamsMap {
        {"css", css},
        {"jsutil", jsUtil},
        {"service", serviceDescr->name},
        {"operation", opDescr->name},
        {"serviceHref", "<a href=\"/ngrest/service/" + serviceDescr->name + "\">"
                        + serviceDescr->name + "</a>"},
        {"operationHref", "<a href=\"/ngrest/operation/" + serviceDescr->name + "/"
                          + opDescr->name + "\">" + opDescr->name + "</a>"},
        {"method", opDescr->methodStr},
        {"location", "<a href='/" + serviceLocation + "/" + opDescr->location
         + "'>" + opDescr->location + "</a>"},
        {"asynchronous", opDescr->asynchronous ? "true" : "false"},
        {"form", form},
        {"jsform", jsFrom},
    });

    pool.putCString(templ.c_str());

    context.callback->success();
}

}
