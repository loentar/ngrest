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

#ifndef NGREST_XML_XMLWRITER_H
#define NGREST_XML_XMLWRITER_H

#include <string>
#include <fstream>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

class Document;
class Declaration;
class Node;
class Element;
class Comment;
class Text;
class Cdata;
class Attribute;
class Namespace;

//! xml writer
class NGREST_XML_EXPORT XmlWriter
{
public:
    //! constructor
    /*! \param  stream - output stream
        \param  enableFormatting - produce human-readable xml
      */
    XmlWriter(std::ostream& stream, bool enableFormatting = true);

    void writeDocument(const Document& document);
    void writeDeclaration(const Declaration& declaration);
    void writeNode(const Node& node);
    void writeElement(const Element& element);
    void writeComment(const Comment& comment);
    void writeText(const Text& text);
    void writeCdata(const Cdata& cdata);
    void writeAttribute(const Attribute& attribute);
    void writeNamespace(const Namespace& ns);

private:
    std::string escapeString(std::string str);
    void writeIndent();
    void writeNewLine();

private:
    std::ostream& stream;
    bool enableFormatting;
    unsigned indent;
};


//! xml file writer
class NGREST_XML_EXPORT XmlFileWriter: public XmlWriter
{
public:
    XmlFileWriter(const std::string& fileName, bool enableFormatting = true);

private:
    std::ofstream stream;
};

} // namespace xml
} // namespace ngrest

#endif // #ifndef NGREST_XML_XMLWRITER_H
