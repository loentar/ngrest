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

#ifndef NGREST_SERVER_H
#define NGREST_SERVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct ngrest_server_request
{
    void* req;               // web-server specific data
    const char* method;      // request method
    const char* uri;         // request uri
    const char* clientHost;  // client host
    const char* clientPort;  // client port
    int hasBody;             // nonzero if request has body
    int64_t bodySize;        // body size or 0 if size is not known
};

typedef int(*iterate_request_headers_callback)(void* context, const char* header, const char* value);

struct ngrest_mod_callbacks
{
    void (*iterate_request_headers)(void* req, void* context, iterate_request_headers_callback callback);
    int64_t (*read_block)(void* req, char* buffer, int64_t size);

    void (*set_response_header)(void* req, const char* header, const char* value);
    void (*set_content_type)(void* req, const char* contentType);
    void (*set_content_length)(void* req, int64_t contentLength);
    int64_t (*write_block)(void* req, const void* buffer, int64_t size);
    void (*finalize_response)(void* req, int statusCode);
};

int ngrest_server_startup(const char* servicesPath);
int ngrest_server_shutdown();
int ngrest_server_dispatch(struct ngrest_server_request* request, struct ngrest_mod_callbacks callbacks);

#ifdef __cplusplus
}
#endif

#endif // NGREST_SERVER_H
