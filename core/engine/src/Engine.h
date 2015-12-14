#ifndef NGREST_ENGINE_H
#define NGREST_ENGINE_H

#include <ngrest/utils/Callback.h>

namespace ngrest {

struct HttpRequest;
struct HttpResponse;
class Exception;

class HttpResponseCallback: public Callback<const HttpResponse*, const Exception&>
{
};

class Engine
{
public:
    Engine();

    void processRequest(HttpRequest* request, HttpResponseCallback* callback);
};

} // namespace ngrest

#endif // NGREST_ENGINE_H
