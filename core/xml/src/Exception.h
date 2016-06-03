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

#ifndef NGREST_XML_EXCEPTION_H
#define NGREST_XML_EXCEPTION_H

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/tostring.h>
#include "ngrestxmlexport.h"

/**
 * @brief throw xml exception
 */
#define NGREST_XML_THROW(DESCRIPTION, XMLFILE, XMLLINE) \
    throw ::ngrest::xml::XmlException(NGREST_FILE_LINE, __FUNCTION__, DESCRIPTION, XMLFILE, XMLLINE);

/**
 * @brief assert expression
 */
#define NGREST_XML_ASSERT(EXPRESSION, DESCRIPTION, XMLFILE, XMLLINE) \
    if (!(EXPRESSION)) NGREST_XML_THROW(DESCRIPTION, XMLFILE, XMLLINE)

namespace ngrest {
namespace xml {

/**
 * @brief ngrest exception
 */
class XmlException: public ::ngrest::Exception
{
public:
    /**
     * @brief exception constructor
     * @param fileLine - source file name and line number
     * @param function - function signature
     * @param description - description
     * @param xmlFile - xml file name where error occured
     * @param xmlLine - xml file line where error occured
     */
    inline XmlException(const char* fileLine, const char* function, std::string description,
                        const std::string& xmlFile, int xmlLine):
        Exception(fileLine, function, description.append(": ")
                  .append(xmlFile).append(":").append(toString(xmlLine)))
    {
    }
};

} // namespace xml
} // namespace ngrest

#endif // #ifndef NGREST_XML_EXCEPTION_H
