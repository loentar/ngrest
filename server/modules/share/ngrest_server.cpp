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

#include <unistd.h>
#include <limits.h>
#include <string.h>

#include <exception>
#include <fstream>

#include <core/utils/Exception.h>
#include <core/utils/Log.h>
#include <core/utils/MemPool.h>
#include <core/utils/MemPooler.h>
#include <core/utils/tocstring.h>
#include <ngrest/engine/Phase.h>
#include <ngrest/engine/Engine.h>
#include <ngrest/engine/FilterDispatcher.h>
#include <ngrest/engine/FilterDeployment.h>
#include <ngrest/engine/ServiceDispatcher.h>
#include <ngrest/engine/Deployment.h>
#include <ngrest/engine/HttpTransport.h>
#include <ngrest/common/HttpMessage.h>
#include <ngrest/common/HttpException.h>

#include "ngrest_server.h"

static ngrest::ServiceDispatcher dispatcher;
static ngrest::Deployment deployment(dispatcher);
static ngrest::FilterDispatcher filterDispatcher;
static ngrest::FilterDeployment filterDeployment(filterDispatcher);
static ngrest::HttpTransport transport;
static ngrest::Engine engine(dispatcher);
static ngrest::MemPooler pooler;
static std::string deployedServicesPath;
static std::string deployedFiltersPath;
static std::ofstream logstream;

struct HeaderIterateContext
{
    ngrest::HttpRequest* httpRequest;
    ngrest::Header* lastHeader;
    ngrest::MessageContext* context;
};


class ModMessageCallback: public ngrest::MessageCallback
{
public:
    ModMessageCallback(ngrest::HttpResponse* httpResponse_):
        httpResponse(httpResponse_)
    {
    }

    void success() override
    {
        if (httpResponse->statusCode == ngrest::HTTP_STATUS_UNDEFINED)
            httpResponse->statusCode = ngrest::HTTP_STATUS_200_OK;
    }

    void error(const ngrest::Exception& error) override
    {
        if (httpResponse->statusCode == ngrest::HTTP_STATUS_UNDEFINED)
            httpResponse->statusCode = ngrest::HTTP_STATUS_500_INTERNAL_SERVER_ERROR;
        httpResponse->poolBody->reset();
        httpResponse->poolBody->putCString(error.what());
    }

    ngrest::HttpResponse* httpResponse;
};

class PoolRecycler {
public:
    PoolRecycler(ngrest::MemPool* pool_):
        pool(pool_)
    {
    }

    ~PoolRecycler()
    {
        pooler.recycle(pool);
    }

private:
    ngrest::MemPool* pool;
};

inline void toLowerCase(char* str)
{
    for (; *str; ++str)
        *str = tolower(*str);
}

int ngrest_server_startup(const char* servicesPath, const char* filtersPath)
{
    if (!servicesPath || !servicesPath[0])
        return 1;

    try {
#ifdef NGREST_MOD_DEBUG
        char logPath[PATH_MAX];
        snprintf(logPath, PATH_MAX, "/tmp/ngrest_%d.log", getpid());
        logstream.open(logPath, std::ios_base::out | std::ios_base::app);
        ngrest::Log::inst().setLogStream(&logstream);
        ngrest::Log::inst().setLogStreamErr(&logstream);
        ngrest::Log::inst().setLogLevel(ngrest::Log::LogLevelVerbose);
#endif

        if (!deployedFiltersPath.empty())
            filterDeployment.undeployAll();

        if (!deployedServicesPath.empty())
            deployment.undeployAll();

        std::string path = servicesPath;
        if (path.back() != '/')
            path.append("/");
        deployedServicesPath = path;

        deployment.deployAll(path);
        if (filtersPath) {
            engine.setFilterDispatcher(&filterDispatcher);
            path = filtersPath;
            if (path.back() != '/')
                path.append("/");
            filterDeployment.deployAll(path);
            deployedFiltersPath = path;
        }

    } catch (const std::exception& e) {
        ngrest::LogWarning() << e.what();
        return 2;
    } catch (...) {
        ngrest::LogWarning() << "Unknown exception raised";
        return 2;
    }

    return 0;
}

int ngrest_server_shutdown()
{
    try {
        if (!deployedServicesPath.empty())
            deployment.undeployAll();
    } catch (const std::exception& e) {
        ngrest::LogWarning() << e.what();
        return 1;
    } catch (...) {
        ngrest::LogWarning() << "Unknown exception raised";
        return 1;
    }

    return 0;
}

void ngrest_server_write_response(ngrest_server_request* request, ngrest_mod_callbacks callbacks, ngrest::MemPool* poolResponse)
{
    uint64_t bodySize = poolResponse->getSize();
    callbacks.set_content_length(request->req, bodySize);

    int chunkCount = poolResponse->getChunkCount();
    const ngrest::MemPool::Chunk* chunk = poolResponse->getChunks();
    for (int i = 0; i < chunkCount; ++i, ++chunk) {
        uint64_t written = 0;
        while (written < chunk->size) {
            int64_t res = callbacks.write_block(request->req, chunk->buffer + written, chunk->size - written);
            NGREST_ASSERT(res > 0, "Failed to write block")
            written += res;
        }
    }
}

int ngrest_server_dispatch(ngrest_server_request* request, ngrest_mod_callbacks callbacks)
{
    try {
        ngrest::MessageContext context;

        context.pool = pooler.obtain();
        PoolRecycler recyclerContext(context.pool);

        context.engine = &engine;
        context.transport = &transport;
        context.request = context.pool->alloc<ngrest::HttpRequest>();
        context.response = context.pool->alloc<ngrest::HttpResponse>();
        context.response->poolBody = pooler.obtain(65536); // 64 KB chunks for output buffer
        PoolRecycler recyclerBody(context.response->poolBody);

        ngrest::HttpRequest* httpRequest = static_cast<ngrest::HttpRequest*>(context.request);
        ngrest::HttpResponse* httpResponse = static_cast<ngrest::HttpResponse*>(context.response);

        ModMessageCallback messageCallback(httpResponse);
        context.callback = &messageCallback;

        httpRequest->setMethod(request->method);
        httpRequest->path = request->uri;
        httpRequest->clientHost = request->clientHost;
        httpRequest->clientPort = request->clientPort;

        HeaderIterateContext itCtx = {
            httpRequest,
            nullptr,
            &context
        };

        callbacks.iterate_request_headers(request->req, &itCtx, [](void* context, const char* name, const char* value) {
            HeaderIterateContext* itCtx = reinterpret_cast<HeaderIterateContext*>(context);
            ngrest::Header* header = itCtx->context->pool->alloc<ngrest::Header>();

            char* headerName = itCtx->context->pool->putCString(name, true);
            toLowerCase(headerName);

            header->name = headerName;
            header->value = value;
            if (itCtx->lastHeader == nullptr) {
                itCtx->httpRequest->headers = header;
            } else {
                itCtx->lastHeader->next = header;
            }
            itCtx->lastHeader = header;
            return 1;
        });

        // TODO: place this before actual body reading
        engine.runPhase(ngrest::Phase::Header, &context);

        if (request->hasBody) {
            ngrest::MemPool* poolRequest = pooler.obtain(65536);
            PoolRecycler recyclerRequest(poolRequest);

            if (request->bodySize > 0) {
                // body size is known
                int64_t bufferSize = request->bodySize;
                poolRequest->reserve(bufferSize + 1); // '\0'

                char* buffer = poolRequest->grow(bufferSize);
                while (bufferSize) {
                    int64_t read = callbacks.read_block(request->req, buffer, bufferSize);
                    NGREST_ASSERT(read <= bufferSize, "read > buffer size");
                    if (read < 0) {
                        callbacks.write_block(request->req, "Failed to read block!", 21);
                        return 500;
                    }

                    bufferSize -= read;
                    buffer += read;
                }
            } else {
                // body size is unknown
                const int64_t chunkSize = poolRequest->getChunkSize();
                int64_t bufferSize = chunkSize;
                char* buffer = poolRequest->grow(chunkSize);
                for (;;) {
                    int64_t read = callbacks.read_block(request->req, buffer, bufferSize);
                    NGREST_ASSERT(read <= bufferSize, "read > buffer size");
                    if (read < 0) {
                        callbacks.write_block(request->req, "Failed to read block!", 21);
                        return 500;
                    }

                    if (!read) // eof
                        break;

                    bufferSize -= read;
                    buffer += read;
                    if (bufferSize == 0) {
                        buffer = poolRequest->grow(chunkSize);
                        bufferSize = chunkSize;
                    }
                }

                poolRequest->shrinkLastChunk(bufferSize);
            }

            ngrest::MemPool::Chunk* body = poolRequest->flatten();
            context.request->body = body->buffer;
            context.request->bodySize = body->size;
            context.request->poolBody = poolRequest;

            engine.dispatchMessage(&context); // poolRequest will be recycled ouside of this scope
        } else {
            engine.dispatchMessage(&context);
        }

        for (const ngrest::Header* header = httpResponse->headers; header; header = header->next) {
            if (!strcmp(header->name, "Content-Type")) {
                callbacks.set_content_type(request->req, header->value);
            } else {
                callbacks.set_response_header(request->req, header->name, header->value);
            }
        }

        ngrest_server_write_response(request, callbacks, httpResponse->poolBody);

        callbacks.finalize_response(request->req, httpResponse->statusCode);

        return 0;
    } catch (const ngrest::HttpException& e) {
        const char* error = e.what();
        ngrest::LogWarning() << error;
        callbacks.write_block(request->req, error, strlen(error));
        callbacks.finalize_response(request->req, e.getHttpStatus());
        return e.getHttpStatus();
    } catch (const std::exception& e) {
        const char* error = e.what();
        ngrest::LogWarning() << error;
        callbacks.write_block(request->req, error, strlen(error));
        callbacks.finalize_response(request->req, 500);
    } catch (...) {
        ngrest::LogWarning() << "Unknown exception raised";
        callbacks.write_block(request->req, "Internal server error", 22);
        callbacks.finalize_response(request->req, 500);
    }

    return 500;
}
