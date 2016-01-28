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

#include "HttpStatus.h"

namespace ngrest {

const char* HttpStatusInfo::httpStatusToString(HttpStatus status)
{
    switch (status) {
    // 1xx: Informational
    case HTTP_STATUS_100_CONTINUE: return "100 Continue";
    case HTTP_STATUS_101_SWITCHING_PROTOCOLS: return "101 Switching Protocols";
    case HTTP_STATUS_102_PROCESSING: return "102 Processing";
    case HTTP_STATUS_105_NAME_NOT_RESOLVED: return "105 Name Not Resolved";

    // 2xx: Success
    case HTTP_STATUS_200_OK: return "200 OK";
    case HTTP_STATUS_201_CREATED: return "201 Created";
    case HTTP_STATUS_202_ACCEPTED: return "202 Accepted";
    case HTTP_STATUS_203_NON_AUTHORITATIVE_INFORMATION: return "203 Non-Authoritative Information";
    case HTTP_STATUS_204_NO_CONTENT: return "204 No Content";
    case HTTP_STATUS_205_RESET_CONTENT: return "205 Reset Content";
    case HTTP_STATUS_206_PARTIAL_CONTENT: return "206 Partial Content";
    case HTTP_STATUS_207_MULTI_STATUS: return "207 Multi-Status";
    case HTTP_STATUS_226_IM_USED: return "226 IM Used";

    // 3xx: Redirection
    case HTTP_STATUS_300_MULTIPLE_CHOICES: return "300 Multiple Choices";
    case HTTP_STATUS_301_MOVED_PERMANENTLY: return "301 Moved Permanently";
    case HTTP_STATUS_302_MOVED_TEMPORARILY: return "302 Moved Temporarily";
    case HTTP_STATUS_303_SEE_OTHER: return "303 See Other";
    case HTTP_STATUS_304_NOT_MODIFIED: return "304 Not Modified";
    case HTTP_STATUS_305_USE_PROXY: return "305 Use Proxy";
    case HTTP_STATUS_307_TEMPORARY_REDIRECT: return "307 Temporary Redirect";

    // 4xx: Client Error
    case HTTP_STATUS_400_BAD_REQUEST: return "400 Bad Request";
    case HTTP_STATUS_401_UNAUTHORIZED: return "401 Unauthorized";
    case HTTP_STATUS_402_PAYMENT_REQUIRED: return "402 Payment Required";
    case HTTP_STATUS_403_FORBIDDEN: return "403 Forbidden";
    case HTTP_STATUS_404_NOT_FOUND: return "404 Not Found";
    case HTTP_STATUS_405_METHOD_NOT_ALLOWED: return "405 Method Not Allowed";
    case HTTP_STATUS_406_NOT_ACCEPTABLE: return "406 Not Acceptable";
    case HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED: return "407 Proxy Authentication Required";
    case HTTP_STATUS_408_REQUEST_TIMEOUT: return "408 Request Timeout";
    case HTTP_STATUS_409_CONFLICT: return "409 Conflict";
    case HTTP_STATUS_410_GONE: return "410 Gone";
    case HTTP_STATUS_411_LENGTH_REQUIRED: return "411 Length Required";
    case HTTP_STATUS_412_PRECONDITION_FAILED: return "412 Precondition Failed";
    case HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE: return "413 Request Entity Too Large";
    case HTTP_STATUS_414_REQUEST_URI_TOO_LARGE: return "414 Request-URI Too Large";
    case HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE: return "415 Unsupported Media Type";
    case HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIABLE: return "416 Requested Range Not Satisfiable";
    case HTTP_STATUS_417_EXPECTATION_FAILED: return "417 Expectation Failed";
    case HTTP_STATUS_418_IM_A_TEAPOT: return "418 I'm a teapot";
    case HTTP_STATUS_422_UNPROCESSABLE_ENTITY: return "422 Unprocessable Entity";
    case HTTP_STATUS_423_LOCKED: return "423 Locked";
    case HTTP_STATUS_424_FAILED_DEPENDENCY: return "424 Failed Dependency";
    case HTTP_STATUS_425_UNORDERED_COLLECTION: return "425 Unordered Collection";
    case HTTP_STATUS_426_UPGRADE_REQUIRED: return "426 Upgrade Required";
    case HTTP_STATUS_428_PRECONDITION_REQUIRED: return "428 Precondition Required";
    case HTTP_STATUS_429_TOO_MANY_REQUESTS: return "429 Too Many Requests";
    case HTTP_STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE: return "431 Request Header Fields Too Large";
    case HTTP_STATUS_434_REQUESTED_HOST_UNAVAILABLE: return "434 Requested host unavailable.";
    case HTTP_STATUS_449_RETRY_WITH: return "449 Retry With";
    case HTTP_STATUS_451_UNAVAILABLE_FOR_LEGAL_REASONS: return "451 Unavailable For Legal Reasons";
    case HTTP_STATUS_456_UNRECOVERABLE_ERROR: return "456 Unrecoverable Error";

    // 5xx: Server Error
    case HTTP_STATUS_500_INTERNAL_SERVER_ERROR: return "500 Internal Server Error";
    case HTTP_STATUS_501_NOT_IMPLEMENTED: return "501 Not Implemented";
    case HTTP_STATUS_502_BAD_GATEWAY: return "502 Bad Gateway";
    case HTTP_STATUS_503_SERVICE_UNAVAILABLE: return "503 Service Unavailable";
    case HTTP_STATUS_504_GATEWAY_TIMEOUT: return "504 Gateway Timeout";
    case HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED: return "505 HTTP Version Not Supported";
    case HTTP_STATUS_506_VARIANT_ALSO_NEGOTIATES: return "506 Variant Also Negotiates";
    case HTTP_STATUS_507_INSUFFICIENT_STORAGE: return "507 Insufficient Storage";
    case HTTP_STATUS_508_LOOP_DETECTED: return "508 Loop Detected";
    case HTTP_STATUS_509_BANDWIDTH_LIMIT_EXCEEDED: return "509 Bandwidth Limit Exceeded";
    case HTTP_STATUS_510_NOT_EXTENDED: return "510 Not Extended";
    case HTTP_STATUS_511_NETWORK_AUTHENTICATION_REQUIRED: return "511 Network Authentication Required";

    default: return "Unknown";
    }

}

}
