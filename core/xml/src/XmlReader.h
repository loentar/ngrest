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

#ifndef NGREST_XML_XMLREADER_H
#define NGREST_XML_XMLREADER_H

#include <string>
#include <fstream>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

class Document;
class Declaration;
class Element;

/**
 * @brief xml reader
 */
class NGREST_XML_EXPORT XmlReader
{
public:
    /**
     * @brief constructor
     * @param stream - input stream
     * @param stripWhitespace ignore xml indentation, trim whitespaces between nodes
     */
    XmlReader(std::istream& stream, bool stripWhitespace = true);

    /**
     * @brief destructor
     */
    ~XmlReader();

    /**
     * @brief set xml document encoding
     *   if document encoding is set, encoding value from declaration will be overriden
     *   else will be used value from declaration or "UTF-8" if encoding attribute is missing
     * @param encoding - xml document encoding
     */
    void setEncoding(const std::string& encoding = "");

    /**
     * @brief set file name
     * @param fileName - file name
     */
    void setFileName(const std::string& fileName);

    /**
     * @brief read xml document
     * @param document - xml document
     */
    void readDocument(Document& document);

    /**
     * @brief read xml declaration
     * @param declaration - xml declaration
     */
    void readDeclaration(Declaration& declaration);

    /**
     * @brief read element
     * @param element - element
     */
    void readElement(Element& element);

private:
    XmlReader(const XmlReader&);
    XmlReader& operator=(const XmlReader&);

private:
    class XmlReaderImpl;
    XmlReaderImpl* impl;
};


/**
 * @brief xml file reader
 */
class NGREST_XML_EXPORT XmlFileReader: public XmlReader
{
public:
    /**
     * @brief constructor
     * @param fileName - file name
     * @param stripWhitespace - ignore human-readable xml formatting
     */
    XmlFileReader(const std::string& fileName, bool stripWhitespace = true);

private:
    std::ifstream stream;
};

} // namespace xml
} // namespace ngrest

#endif // #ifndef NGREST_XML_XMLREADER_H
