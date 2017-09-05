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

#ifndef NGREST_HTTPSTATUS_H
#define NGREST_HTTPSTATUS_H

#include "ngrestcommonexport.h"

namespace ngrest {

/**
 * @brief HTTP status code
 */
enum HttpStatus
{
    HTTP_STATUS_UNDEFINED,

    // 1xx: Informational
    HTTP_STATUS_100_CONTINUE = 100, // Continue
    HTTP_STATUS_101_SWITCHING_PROTOCOLS = 101, // Switching Protocols
    HTTP_STATUS_102_PROCESSING = 102, // Processing
    HTTP_STATUS_105_NAME_NOT_RESOLVED = 105, // Name Not Resolved

    // 2xx: Success
    HTTP_STATUS_200_OK = 200, // OK
    HTTP_STATUS_201_CREATED = 201, // Created
    HTTP_STATUS_202_ACCEPTED = 202, // Accepted
    HTTP_STATUS_203_NON_AUTHORITATIVE_INFORMATION = 203, // Non-Authoritative Information
    HTTP_STATUS_204_NO_CONTENT = 204, // No Content
    HTTP_STATUS_205_RESET_CONTENT = 205, // Reset Content
    HTTP_STATUS_206_PARTIAL_CONTENT = 206, // Partial Content
    HTTP_STATUS_207_MULTI_STATUS = 207, // Multi-Status
    HTTP_STATUS_226_IM_USED = 226, // IM Used

    // 3xx: Redirection
    HTTP_STATUS_300_MULTIPLE_CHOICES = 300, // Multiple Choices
    HTTP_STATUS_301_MOVED_PERMANENTLY = 301, // Moved Permanently
    HTTP_STATUS_302_MOVED_TEMPORARILY = 302, // Moved Temporarily
    HTTP_STATUS_303_SEE_OTHER = 303, // See Other
    HTTP_STATUS_304_NOT_MODIFIED = 304, // Not Modified
    HTTP_STATUS_305_USE_PROXY = 305, // Use Proxy
    HTTP_STATUS_307_TEMPORARY_REDIRECT = 307, // Temporary Redirect

    // 4xx: Client Error
    HTTP_STATUS_400_BAD_REQUEST = 400, // Bad Request
    HTTP_STATUS_401_UNAUTHORIZED = 401, // Unauthorized
    HTTP_STATUS_402_PAYMENT_REQUIRED = 402, // Payment Required
    HTTP_STATUS_403_FORBIDDEN = 403, // Forbidden
    HTTP_STATUS_404_NOT_FOUND = 404, // Not Found
    HTTP_STATUS_405_METHOD_NOT_ALLOWED = 405, // Method Not Allowed
    HTTP_STATUS_406_NOT_ACCEPTABLE = 406, // Not Acceptable
    HTTP_STATUS_407_PROXY_AUTHENTICATION_REQUIRED = 407, // Proxy Authentication Required
    HTTP_STATUS_408_REQUEST_TIMEOUT = 408, // Request Timeout
    HTTP_STATUS_409_CONFLICT = 409, // Conflict
    HTTP_STATUS_410_GONE = 410, // Gone
    HTTP_STATUS_411_LENGTH_REQUIRED = 411, // Length Required
    HTTP_STATUS_412_PRECONDITION_FAILED = 412, // Precondition Failed
    HTTP_STATUS_413_REQUEST_ENTITY_TOO_LARGE = 413, // Request Entity Too Large
    HTTP_STATUS_414_REQUEST_URI_TOO_LARGE = 414, // Request-URI Too Large
    HTTP_STATUS_415_UNSUPPORTED_MEDIA_TYPE = 415, // Unsupported Media Type
    HTTP_STATUS_416_REQUESTED_RANGE_NOT_SATISFIABLE = 416, // Requested Range Not Satisfiable
    HTTP_STATUS_417_EXPECTATION_FAILED = 417, // Expectation Failed
    HTTP_STATUS_418_IM_A_TEAPOT = 418, // I'm a teapot
    HTTP_STATUS_422_UNPROCESSABLE_ENTITY = 422, // Unprocessable Entity
    HTTP_STATUS_423_LOCKED = 423, // Locked
    HTTP_STATUS_424_FAILED_DEPENDENCY = 424, // Failed Dependency
    HTTP_STATUS_425_UNORDERED_COLLECTION = 425, // Unordered Collection
    HTTP_STATUS_426_UPGRADE_REQUIRED = 426, // Upgrade Required
    HTTP_STATUS_428_PRECONDITION_REQUIRED = 428, // Precondition Required
    HTTP_STATUS_429_TOO_MANY_REQUESTS = 429, // Too Many Requests
    HTTP_STATUS_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431, // Request Header Fields Too Large
    HTTP_STATUS_434_REQUESTED_HOST_UNAVAILABLE = 434, // Requested host unavailable.
    HTTP_STATUS_449_RETRY_WITH = 449, // Retry With
    HTTP_STATUS_451_UNAVAILABLE_FOR_LEGAL_REASONS = 451, // Unavailable For Legal Reasons
    HTTP_STATUS_456_UNRECOVERABLE_ERROR = 456, // Unrecoverable Error

    // 5xx: Server Error
    HTTP_STATUS_500_INTERNAL_SERVER_ERROR = 500, // Internal Server Error
    HTTP_STATUS_501_NOT_IMPLEMENTED = 501, // Not Implemented
    HTTP_STATUS_502_BAD_GATEWAY = 502, // Bad Gateway
    HTTP_STATUS_503_SERVICE_UNAVAILABLE = 503, // Service Unavailable
    HTTP_STATUS_504_GATEWAY_TIMEOUT = 504, // Gateway Timeout
    HTTP_STATUS_505_HTTP_VERSION_NOT_SUPPORTED = 505, // HTTP Version Not Supported
    HTTP_STATUS_506_VARIANT_ALSO_NEGOTIATES = 506, // Variant Also Negotiates
    HTTP_STATUS_507_INSUFFICIENT_STORAGE = 507, // Insufficient Storage
    HTTP_STATUS_508_LOOP_DETECTED = 508, // Loop Detected
    HTTP_STATUS_509_BANDWIDTH_LIMIT_EXCEEDED = 509, // Bandwidth Limit Exceeded
    HTTP_STATUS_510_NOT_EXTENDED = 510, // Not Extended
    HTTP_STATUS_511_NETWORK_AUTHENTICATION_REQUIRED = 511 // Network Authentication Required};
};

class NGREST_COMMON_EXPORT HttpStatusInfo {
public:
    static const char* httpStatusToString(HttpStatus status);
};

}

#endif // NGREST_HTTPSTATUS_H
