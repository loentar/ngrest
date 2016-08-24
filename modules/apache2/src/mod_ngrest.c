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
#include <ap_config.h>
#include <httpd.h>
#include <http_config.h>
#include <http_protocol.h>
#include <http_log.h>

#include "../../share/ngrest_server.h"


static const char* ngrestServicesPath = NULL;
static int initialized = 0;

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

#define INT_STR_LEN 16

static void mod_ngrest_iterate_request_headers(void* req, void* data, iterate_request_headers_callback callback)
{
    apr_table_do(callback, data, ((request_rec*)req)->headers_in, NULL);
}

static int64_t mod_ngrest_read_client_callback(void* req, char* buffer, int64_t size)
{
    int64_t res = ap_get_client_block(req, buffer, size);
    LOG2("REMAINING: %ld / %ld", ((request_rec*)req)->remaining, ((request_rec*)req)->read_length);
    LOG1("res: %ld", res);

    return res;
}

static void mod_ngrest_set_response_header(void* req, const char* header, const char* value)
{
    apr_table_set(((request_rec*)req)->headers_out, header, value);
}

static void mod_ngrest_set_content_type(void* req, const char* contentType)
{
    ((request_rec*)req)->content_type = contentType;
}

static void mod_ngrest_set_content_length(void* req, int64_t contentLength)
{
    // apache2 ignores setting of Content-Length header
}

static int64_t mod_ngrest_write_client_callback(void* req, const void* buffer, int64_t size)
{
    LOG1("response size: %ld", size);
    return ap_rwrite(buffer, size, req);
}

static void mod_ngrest_finalize_request(void* req, int status)
{
    ((request_rec*)req)->status = status;
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

/* content handler */
static int mod_ngrest_handler(request_rec* req)
{
    if (strcmp(req->handler, "ngrest"))
        return DECLINED;

    if (!req->header_only) {
        LOGLABEL;

        if (!initialized) {
            if (!ngrestServicesPath) {
                ap_log_error(APLOG_MARK, APLOG_INFO, 0, req->server, "ngrest: ServicesPath is not set");
                return 500;
            }

            int res = ngrest_server_startup(ngrestServicesPath);
            LOG2("server startup: #%d: %s", res, !res ? "OK" : "FAILED");
            if (res) {
                ap_log_error(APLOG_MARK, APLOG_INFO, 0, req->server, "ngrest: server startup failed with code %d", res);
                return 500;
            }

            initialized = 1;
        }

        if (ap_setup_client_block(req, REQUEST_CHUNKED_ERROR) != OK) {
            ap_log_error(APLOG_MARK, APLOG_INFO, 0, req->server, "ngrest: Error ap_setup_client_block");
            return 500;
        }

        LOG("Dispatching request");
        char portStr[INT_STR_LEN] = {0};
        if (snprintf(portStr, INT_STR_LEN, "%d", req->connection->client_addr->port) >= INT_STR_LEN) {
            ap_log_error(APLOG_MARK, APLOG_INFO, 0, req->server, "ngrest: Port buffer overflow");
            return 500;
        }

        struct ngrest_server_request request = {
            req,
            req->method,
            req->unparsed_uri,
            req->connection->client_ip,
            portStr,
            ap_should_client_block(req),
            req->remaining
        };

        ngrest_server_dispatch(&request, mod_ngrest_server_callbacks);
        LOGLABEL;
    }

    return OK;
}

static apr_status_t mod_ngrest_shutdown(void* data)
{
    LOGLABEL;

    if (initialized) {
        ngrest_server_shutdown();
        initialized = 0;
    }

    return APR_SUCCESS;
}

static void ngrest_register_hooks(apr_pool_t* pool)
{
#ifdef NGREST_MOD_DEBUG
    char logPath[PATH_MAX];
    snprintf(logPath, PATH_MAX, "/tmp/ngrest_debug_%d.log", getpid());
    logFile = fopen(logPath, "a");
#endif

    ap_hook_handler(mod_ngrest_handler, NULL, NULL, APR_HOOK_MIDDLE);
    apr_pool_pre_cleanup_register(pool, NULL, mod_ngrest_shutdown);

    LOGLABEL;
}

static const char* ngrest_set_ngrest_services_path(cmd_parms* cmdParams, void* config, const char* path)
{
    LOG1("Using ngrest services path: [%s]", path);
    ngrestServicesPath = path;

    return NULL;
}


static const command_rec ngrest_config_commands[] =
{
    AP_INIT_RAW_ARGS("ServicesPath", ngrest_set_ngrest_services_path, NULL, RSRC_CONF, "Set ngrest services path"),
    {NULL}
};

/* Dispatch list for API hooks */
module AP_MODULE_DECLARE_DATA ngrest_module = {
    STANDARD20_MODULE_STUFF,
    NULL,                     /* create per-dir    config structures */
    NULL,                     /* merge  per-dir    config structures */
    NULL,                     /* create per-server config structures */
    NULL,                     /* merge  per-server config structures */
    ngrest_config_commands,   /* table of config file commands       */
    ngrest_register_hooks     /* register hooks                      */
};

