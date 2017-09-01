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

#ifndef HTTPEXCEPTION_H
#define HTTPEXCEPTION_H

#include <ngrest/utils/Exception.h>
#include "ngrestcommonexport.h"
#include "HttpStatus.h"


//! throw HTTP exception
#define NGREST_THROW_HTTP(DESCRIPTION, STATUS) \
    throw ::ngrest::HttpException(NGREST_FILE_LINE, __FUNCTION__, DESCRIPTION, STATUS);

//! assert HTTP expression
#define NGREST_ASSERT_HTTP(EXPRESSION, STATUS, DESCRIPTION) \
    if (NGREST_UNLIKELY(!(EXPRESSION))) NGREST_THROW_HTTP(DESCRIPTION, STATUS)


namespace ngrest {

/**
 * HTTP exception
 */
class NGREST_COMMON_EXPORT HttpException: public Exception
{
public:
    /**
     * @brief exception constructor
     * @param fileLine source file name and line number
     * @param function function signature
     * @param description description
     * @param status HTTP status code
     */
    HttpException(const char* fileLine, const char* function, const std::string& description,
                  HttpStatus status);

    /**
     * @brief get http status code
     * @return http status code
     */
    inline HttpStatus getHttpStatus() const
    {
        return httpStatus;
    }

private:
    const HttpStatus httpStatus;
};

}

#endif // HTTPEXCEPTION_H
