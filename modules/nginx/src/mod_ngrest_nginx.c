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

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_http.h>

#include "../../share/ngrest_server.h"


static const char* ngrestServicesPath = NULL;
static int initialized = 0;

typedef struct {
    ngx_http_request_t* req;

    ngx_chain_t* inChain;
    int64_t inOffset;

    ngx_chain_t outChain;
    ngx_chain_t* outLastChain;
    ngx_buf_t outFirstBuf;
} mod_ngrest_request_ctx_t;

#ifdef NGREST_MOD_DEBUG
FILE* logFile = NULL;
#define LOGLABEL fprintf(logFile, "p%d t%d; %s[%d]: %s\n", \
    getpid(), (int)pthread_self(), __FILE__, __LINE__, __FUNCTION__); fflush(logFile);

#define LOG(str) fprintf(logFile, "p%d t%d; %s[%d]: %s: %s\n", \
    getpid(), (int)pthread_self(), __FILE__, __LINE__, __FUNCTION__, str); fflush(logFile);
#define LOG1(str, arg1) fprintf(logFile, "p%d t%d; %s[%d]: %s: " str "\n", \
    getpid(), (int)pthread_self(), __FILE__, __LINE__, __FUNCTION__, arg1); fflush(logFile);
#define LOG2(str, arg1, arg2) fprintf(logFile, "p%d t%d; %s[%d]: %s: " str "\n", \
    getpid(), (int)pthread_self(), __FILE__, __LINE__, __FUNCTION__, arg1, arg2); fflush(logFile);
#else
#define LOGLABEL
#define LOG(str)
#define LOG1(str, arg1)
#define LOG2(str, arg1, arg2)
#endif

static char* mod_ngrest_to_cstring(const ngx_str_t* str, ngx_pool_t* pool)
{
    char* res = ngx_pcalloc(pool, str->len + 1);
    strncpy(res, str->data, str->len);
    str->data[str->len] = '\0';
    return res;
}

static void mod_ngrest_to_ngx_string(const char* src, ngx_str_t* dst)
{
    dst->data = (u_char*) src;
    dst->len = strlen(src);
}

static void mod_ngrest_iterate_request_headers(void* req, void* context, iterate_request_headers_callback callback)
{
    ngx_http_request_t* r = ((mod_ngrest_request_ctx_t*) req)->req;

    const ngx_list_part_t* part = &r->headers_in.headers.part;
    ngx_table_elt_t *headers = part->elts;
    int i;

    for (i = 0;; ++i) {

        if (i >= part->nelts) {
            if (!part->next)
                break;

            part = part->next;
            headers = part->elts;
            i = 0;
        }

        if (headers[i].hash)
            callback(context, mod_ngrest_to_cstring(&(headers[i].key), r->pool),
                     mod_ngrest_to_cstring(&(headers[i].value), r->pool));
    }
}

static int64_t mod_ngrest_read_client_callback(void* req, char* buffer, int64_t bufferSize)
{
    mod_ngrest_request_ctx_t* ctx = (mod_ngrest_request_ctx_t*) req;
    ngx_http_request_t* r = ((mod_ngrest_request_ctx_t*) req)->req;

    int64_t res = 0;

    if (!r->request_body->temp_file) {
        LOG("Reading data from buffers...");

        for (; ctx->inChain; ctx->inChain = ctx->inChain->next) {
            ngx_buf_t* inBuf = ctx->inChain->buf;

            int64_t inBufSize = inBuf->last - inBuf->pos - ctx->inOffset;
            LOG2("bufSize: %ld, inSize: %ld", bufferSize, inBufSize);
            LOG1("offset: %ld", ctx->inOffset);
            if (inBufSize <= 0) {
                LOG1("SIZE <= 0: %ld", inBufSize);
                return -1;
            }

            if (inBufSize <= bufferSize) {
                memcpy(buffer, inBuf->pos + ctx->inOffset, inBufSize);
                res += inBufSize;
                bufferSize -= inBufSize;
                buffer += inBufSize;
                ctx->inOffset = 0;
            } else {
                memcpy(buffer, inBuf->pos + ctx->inOffset, bufferSize);
                ctx->inOffset += bufferSize;
                return bufferSize;
            }
        }
    } else {
        LOG("Reading data from file...");
        LOG1("bufSize: %ld", bufferSize);

        res = ngx_read_file(&r->request_body->temp_file->file, buffer, bufferSize, ctx->inOffset);
        LOG1("read: %ld", res);
        ctx->inOffset += res;
    }

    return res;
}

static void mod_ngrest_set_response_header(void* req, const char* header, const char* value)
{
    ngx_http_request_t* r = ((mod_ngrest_request_ctx_t*) req)->req;
    ngx_table_elt_t* table = ngx_list_push(&r->headers_out.headers);

    if (table) {
        table->hash = (ngx_uint_t) header;
        mod_ngrest_to_ngx_string(header, &table->key);
        mod_ngrest_to_ngx_string(value, &table->value);
    }
}

static void mod_ngrest_set_content_type(void* req, const char* contentType)
{
    mod_ngrest_to_ngx_string(contentType, &((mod_ngrest_request_ctx_t*) req)->req->headers_out.content_type);
}

static void mod_ngrest_set_content_length(void* req, int64_t contentLength)
{
    ((mod_ngrest_request_ctx_t*) req)->req->headers_out.content_length_n = contentLength;
}

static int64_t mod_ngrest_write_client_callback(void* req, const void* buffer, int64_t size)
{
    LOG1("response size: %ld", size);
    mod_ngrest_request_ctx_t* ctx = (mod_ngrest_request_ctx_t*) req;

    ngx_buf_t* buf;

    ngx_chain_t* chain;
    if (!ctx->outLastChain) {
        chain = &ctx->outChain;
        buf = &ctx->outFirstBuf;
    } else {
        chain = ngx_pcalloc(ctx->req->pool, sizeof(ngx_chain_t));
        ctx->outLastChain->next = chain;
        buf = ngx_pcalloc(ctx->req->pool, sizeof(ngx_buf_t));
    }
    chain->buf = buf;
    chain->next = NULL;
    ctx->outLastChain = chain;

    buf->pos = (u_char*) buffer;          // buffer chain begin
    buf->last = (u_char*) buffer + size;  // buffer chain end
    buf->memory = 1;                      // is buffer read-only
    buf->last_buf = 0;                    // not the last chain

    return size;
}

static void mod_ngrest_finalize_request(void* req, int status)
{
    mod_ngrest_request_ctx_t* ctx = (mod_ngrest_request_ctx_t*) req;
    ngx_http_request_t* r = ctx->req;

    r->headers_out.status = status;
    LOGLABEL;

    ngx_http_send_header(r);
    LOGLABEL;

    // terminate body
    if (ctx->outLastChain) {
        LOGLABEL;
        ctx->outLastChain->buf->last_buf = 1; // mark the last chain
    } else {
        // send empty response
        ctx->outChain.buf = &ctx->outFirstBuf;
        ctx->outFirstBuf.last_buf = 1;
    }

    LOGLABEL;
    ngx_int_t res = ngx_http_output_filter(r, &ctx->outChain);
    LOG1("RC: %ld", res);
}

static struct ngrest_mod_callbacks mod_ngrest_server_callbacks = {
    mod_ngrest_iterate_request_headers,
    mod_ngrest_read_client_callback,
    mod_ngrest_set_response_header,
    mod_ngrest_set_content_type,
    mod_ngrest_set_content_length,
    mod_ngrest_write_client_callback,
    mod_ngrest_finalize_request
};


static void mod_ngrest_handler(ngx_http_request_t* req)
{
    LOGLABEL;

    if (!initialized) {
        if (!ngrestServicesPath) {
            LOG("ngrestServicesPath = NULL");
            ngx_log_debug0(NGX_LOG_DEBUG_HTTP, req->connection->log, 0, "ngrestServicesPath = NULL");
            return;
        }

        LOG1("Using ngrest services path: %s", ngrestServicesPath);

        if (ngrest_server_startup(ngrestServicesPath)) {
            LOG("failed to start ngrest");
            ngx_log_debug0(NGX_LOG_DEBUG_HTTP, req->connection->log, 0, "failed to start ngrest");
            return;
        }

        LOG("ngrest has been started");
        initialized = 1;
    }


    LOG("Dispatching request");

    char clientHost[NI_MAXHOST];
    char clientPort[NI_MAXSERV];

    int res = getnameinfo(req->connection->sockaddr, req->connection->socklen,
                          clientHost, sizeof(clientHost),
                          clientPort, sizeof(clientPort),
                          NI_NUMERICHOST | NI_NUMERICSERV);
    if (res) {
        clientHost[0] = '\0';
        clientPort[0] = '\0';
        LOG2("Failed to get client host/port: %d : %s", res, strerror(res));
    }


    LOG1("request_length: %ld", req->request_length);
    LOG1("Has body: %p", req->request_body);
    if (req->request_body) {
        LOG1("file: %p", req->request_body->temp_file);
        LOG1("buf: %p", req->request_body->buf);
        LOG1("bufs: %p", req->request_body->bufs);
    }

    mod_ngrest_request_ctx_t ctx;
    ngx_memzero(&ctx, sizeof(ctx));
    ctx.req = req;

    if (req->request_body)
        ctx.inChain = req->request_body->bufs;

    struct ngrest_server_request request = {
        &ctx,
        mod_ngrest_to_cstring(&req->method_name, req->pool),
        mod_ngrest_to_cstring(&req->unparsed_uri, req->pool),
        clientHost,
        clientPort,
        !!req->request_body,
        req->headers_in.content_length_n
    };

    LOG2("Client: %s:%s", clientHost, clientPort);
    ngrest_server_dispatch(&request, mod_ngrest_server_callbacks);
}


static ngx_int_t mod_ngrest_request_handler(ngx_http_request_t* req)
{
    if (req->headers_in.content_length_n > 0) {
        // read body and call handler
        ngx_int_t rc = ngx_http_read_client_request_body(req, mod_ngrest_handler);
        if (rc >= NGX_HTTP_SPECIAL_RESPONSE)
            return rc;

        return NGX_DONE;
    }

    // no request body
    mod_ngrest_handler(req);
    return NGX_OK;
}

static char* mod_ngrest_set_conf(ngx_conf_t* conf, ngx_command_t* cmd, void* unused)
{
    if (conf->args->nelts != 2)  // "ngrest", "/path/to/services"
        return NGX_CONF_ERROR;

#ifdef NGREST_MOD_DEBUG
    char logPath[PATH_MAX];
    snprintf(logPath, PATH_MAX, "/tmp/ngrest_debug_%d.log", getpid());
    logFile = fopen(logPath, "a");
#endif

    ngx_http_core_loc_conf_t* httpConf = ngx_http_conf_get_module_loc_conf(conf, ngx_http_core_module);
    httpConf->handler = mod_ngrest_request_handler;

    ngrestServicesPath = mod_ngrest_to_cstring(&(((ngx_str_t*) conf->args->elts)[1]), conf->pool);
    LOG1("Using ngrest services path: %s", ngrestServicesPath);

    return NGX_CONF_OK;
}

static ngx_command_t mod_ngrest_commands[] = {
    {ngx_string("ngrest"), NGX_HTTP_LOC_CONF | NGX_CONF_TAKE1, mod_ngrest_set_conf, 0, 0, NULL},
    ngx_null_command
};

static ngx_http_module_t mod_ngrest_ctx = {
    NULL, // pre-conf
    NULL, // post-conf
    NULL, // create main conf
    NULL, // init main conf
    NULL, // create server conf
    NULL, // merge server conf
    NULL, // create location conf
    NULL  // merge location conf
};

static ngx_module_t mod_ngrest = {
    NGX_MODULE_V1,
    &mod_ngrest_ctx, // context
    mod_ngrest_commands, // directives
    NGX_HTTP_MODULE, // type
    NULL, // init master
    NULL, // init module
    NULL, // init process
    NULL, // init thread
    NULL, // exit thread
    NULL, // exit process
    NULL, // exit master
    NGX_MODULE_V1_PADDING
};

ngx_module_t* ngx_modules[] = {
    &mod_ngrest,
    NULL
};

char* ngx_module_names[] = {
    "ngrest",
    NULL
};
