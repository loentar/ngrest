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

#ifndef NGREST_XML_ATTRIBUTE_H
#define NGREST_XML_ATTRIBUTE_H

#include <string>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

/**
 * @brief xml-attribute
 */
class NGREST_XML_EXPORT Attribute
{
public:
    /**
     * @brief copy constructor
     * @param attr source attribute
     */
    Attribute(const Attribute& attr);

    /**
     * @brief initializing constructor
     * @param name attribute name
     */
    Attribute(const std::string& name);

    /**
     * @brief initializing constructor
     * @param name attribute name
     * @param value attribute value
     */
    Attribute(const std::string& name, const std::string& value);

    /**
     * @brief initializing constructor
     * @param name attribute name
     * @param value attribute value
     * @param prefix attribute prefix
     */
    Attribute(const std::string& name, const std::string& value, const std::string& prefix);


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
     * @brief get name
     * @return name
     */
    const std::string& getName() const;

    /**
     * @brief set name
     * @param name name
     */
    void setName(const std::string& name);

    /**
     * @brief get attribute name with prefix
     * @return attribute name with prefix
     */
    std::string getPrefixName() const;

    /**
     * @brief get value
     * @return value
     */
    const std::string& getValue() const;

    /**
     * @brief set value
     * @param value value
     */
    void setValue(const std::string& value);


    /**
     * @brief get next sibling attribute
     * @return next sibling attribute
     */
    const Attribute* getNextSibling() const;

    /**
     * @brief get next sibling attribute
     * @return next sibling attribute
     */
    Attribute* getNextSibling();


    /**
     * @brief copy operator
     * @param attr source attribute
     * @return *this
     */
    Attribute& operator=(const Attribute& attr);

    /**
     * @brief check whether the attributes are equal

     * @param attr other attribute
     * @return true, if attributes are equal
     */
    bool operator==(const Attribute& attr) const;

    /**
     * @brief check whether the attributes are not equal

     * @param attr other attribute
     * @return true, if attributes are not equal
     */
    bool operator!=(const Attribute& attr) const;

private:
    std::string prefix;       //!< attribute namespace prefix
    std::string name;         //!< attribute name
    std::string value;       //!< attribute value
    Attribute* nextSibling;   //!< next attribute
    friend class Element;
};

} // namespace xml
} // namespace ngrest

#endif // NGREST_XML_ATTRIBUTE_H

