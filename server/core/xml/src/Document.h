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

#ifndef NGREST_XML_DOCUMENT_H
#define NGREST_XML_DOCUMENT_H

#include "Declaration.h"
#include "Element.h"
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

/**
 * @brief XML Document
 */
class NGREST_XML_EXPORT Document
{
public:
    /**
     * @brief get document declaration
     * @return document declaration
     */
    const Declaration& getDeclaration() const;

    /**
     * @brief get document declaration
     * @return document declaration
     */
    Declaration& getDeclaration();

    /**
     * @brief get root Element
     * @return root Element
     */
    const Element& getRootElement() const;

    /**
     * @brief get root node
     * @return root node
     */
    Element& getRootElement();

private:
    Declaration declaration;  //!< xml-declaration
    Element rootElement;      //!< root element
};

} // namespace xml
} // namespace ngrest


#endif // NGREST_XML_DOCUMENT_H
