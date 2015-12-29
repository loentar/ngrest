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

//! xml reader
class NGREST_XML_EXPORT XmlReader
{
public:
    //! constructor
    /*! \param  stream - input stream
        \param  stripWhitespace - ignore human-readable xml formatting
      */
    XmlReader(std::istream& stream, bool stripWhitespace = true);

    //! destructor
    ~XmlReader();

    //! set xml document encoding
    /*! if document encoding is set, encoding value from declaration will be overriden
        else will be used value from declaration or "UTF-8" if encoding attribute is missing
        \param  encoding - xml document encoding
      */
    void setEncoding(const std::string& encoding = "");

    //! set file name
    /*! \param  fileName - file name
      */
    void setFileName(const std::string& fileName);

    //! read xml document
    /*! \param  document - xml document
      */
    void readDocument(Document& document);

    //! read xml declaration
    /*! \param  declaration - xml declaration
      */
    void readDeclaration(Declaration& declaration);

    //! read element
    /*! \param  element - element
      */
    void readElement(Element& element);

private:
    class XmlReaderImpl;
    XmlReaderImpl* impl;
};


//! xml file writer
class NGREST_XML_EXPORT XmlFileReader: public XmlReader
{
public:
    //! constructor
    /*! \param  fileName - file name
        \param  stripWhitespace - ignore human-readable xml formatting
      */
    XmlFileReader(const std::string& fileName, bool stripWhitespace = true);

private:
    std::ifstream stream;
};

} // namespace xml
} // namespace ngrest

#endif // #ifndef NGREST_XML_XMLREADER_H
