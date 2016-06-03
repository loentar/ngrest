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

#ifndef NGREST_XML_DECLARATION_H
#define NGREST_XML_DECLARATION_H

#include <string>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

/**
 * @brief xml-document declaration
 */
class NGREST_XML_EXPORT Declaration
{
public:
    enum class Standalone //! standalone
    {
        Undefined, //!< not defined
        No,        //!< standalone="no"
        Yes        //!< standalone="yes"
    };

public:
    /**
     * @brief constructor
     */
    Declaration();

    /**
     * @brief destructor
     */
    ~Declaration();

    /**
     * @brief get version
     * @return version
     */
    const std::string& getVersion() const;

    /**
     * @brief set version
     * @param version version
     */
    void setVersion(const std::string& version);


    /**
     * @brief get encoding
     * @return encoding
     */
    const std::string& getEncoding() const;

    /**
     * @brief set encoding
     * @param encoding encoding
     */
    void setEncoding(const std::string& encoding);


    /**
     * @brief get stand alone
     * @return stand alone
     */
    Standalone getStandalone() const;

    /**
     * @brief set stand alone
     * @param standalone standalone
     */
    void setStandalone(Standalone standalone);


private:
    std::string  version;      //!< xml document version(1.0)
    std::string  encoding;     //!< encoding
    Standalone   standalone;   //!< standalone declaration
};

} // namespace xml
} // namespace ngrest

#endif // NGREST_XML_DECLARATION_H

