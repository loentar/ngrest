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

#ifndef NGREST_XML_NAMESPACE_H
#define NGREST_XML_NAMESPACE_H

#include <string>
#include <iosfwd>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

/**
 * @brief xml namespace
 */
class NGREST_XML_EXPORT Namespace
{
public:
    /**
     * @brief copy constructor
     */
    Namespace(const Namespace& ns);

    /**
     * @brief constructor
     * @param prefix prefix
     * @param uri uri
     */
    Namespace(const std::string& prefix, const std::string& uri);


    /**
     * @brief copy operator
     * @param ns source namespace
     * @return *this
     */
    Namespace& operator=(const Namespace& ns);


    /**
     * @brief get prefix
     * @return prefix
     */
    const std::string& getPrefix() const;

    /**
     * @brief set prefix
     * @param prefix prefix
     */
    void setPrefix(const std::string& prefix);


    /**
     * @brief get uri
     * @return uri
     */
    const std::string& getUri() const;

    /**
     * @brief set uri
     * @param uri uri
     */
    void setUri(const std::string& uri);


    /**
     * @brief get next sibling namespace
     * @return next sibling namespace
     */
    const Namespace* getNextSibling() const;

    /**
     * @brief get next sibling namespace
     * @return next sibling namespace
     */
    Namespace* getNextSibling();


    /**
     * @brief check whether the namespaces are equal
     * @param ns other namespace
     * @return true, if namespaces are equal
     */
    bool operator==(const Namespace& ns) const;

    /**
     * @brief check whether the namespaces are not equal
     * @param ns other namespace
     * @return true, if namespaces are not equal
     */
    bool operator!=(const Namespace& ns) const;

private:
    std::string prefix;      //!< prefix
    std::string uri;         //!< uri
    Namespace* nextSibling;  //!< next namespace
    friend class Element;
};

} // namespace xml
} // namespace ngrest

#endif // NGREST_XML_NAMESPACE_H
