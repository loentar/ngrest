#include <string.h>
#include <string>

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/Log.h>

#include <ngrest/json/JsonReader.h>
#include <ngrest/json/JsonWriter.h>
#include <ngrest/json/JsonTypes.h>

#include "Engine.h"
#include "HttpCommon.h"

namespace ngrest {

Engine::Engine()
{

}

void Engine::processRequest(HttpRequest* request, HttpResponseCallback* callback)
{
    try {
        LogDebug() << "Request URL: " << request->url;

        const HttpHeader* contentType = request->getHeader("content-type");
        NGREST_ASSERT(contentType, "Content-Type header is missing!");

        if (!strcmp(contentType->value, "application/json")) {
            // JSON request
            if (request->body != nullptr) {
                MemPool poolJsonIn;
                json::Node* root = json::JsonReader::read(request->body, poolJsonIn);
                NGREST_ASSERT(root, "Failed to read request"); // should never throw

                // FIXME: handle
                if (root->type == json::TypeObject) {
                    json::Object* obj = static_cast<json::Object*>(root);

                    LogDebug() << "firstChild: " << obj->firstChild->name;
                }
                // FIXME: end handle
            }
        } else {
            NGREST_THROW_ASSERT(std::string("Can't handle content type: ") + contentType->value);
        }


//        MemPool poolJsonOut;

        HttpResponse resp;
        HttpHeader headerContentType("Content-Type", "application/json");
        resp.headers = &headerContentType;
        resp.poolBody.putCString("{\"response\": \"This is a test response\"}");
        callback->onSuccess(&resp);
    } catch (const Exception& err) {
        callback->onError(err);
    }
    delete callback;
}

} // namespace ngrest

