#include <streambuf>
#include <ngrest/utils/fromcstring.h>
#include <ngrest/utils/Error.h>
#include "Exception.h"
#include "Document.h"
#include "Attribute.h"
#include "Namespace.h"
#include "Element.h"
#include "Node.h"
#include "XmlReader.h"

namespace ngrest {
namespace xml {

class XmlReader::XmlReaderImpl
{
public:
    XmlReaderImpl(std::istream& stream, bool stripWhitespace):
        stream(stream),
        stripWhitespace(stripWhitespace),
        line(0)
    {
    }

    ~XmlReaderImpl()
    {
    }

    void readDeclaration(Declaration& declaration)
    {
        skipWs();

        declaration.setVersion("1.0");
        declaration.setEncoding("UTF-8");
        declaration.setStandalone(Declaration::Standalone::Undefined);

        if (!test("<?xml"))
            return;

        std::string name;
        std::string value;
        char ch = '\0';
        for (;;) {
            skipWs();
            if (test("?>"))
                break;

            readStringBeforeChr(name, " \n\r\t=");
            skipWs();
            NGREST_XML_ASSERT(test("="), "'=' expected while reading attribute/namespace [" + name + "]",
                              fileName, line);
            skipWs();

            readChar(ch);
            NGREST_XML_ASSERT(ch == '"' || ch == '\'',
                              "quote expected while reading attribute/namespace ["
                              + name + "]", fileName, line);
            readStringWithChr(value, ch);

            if (name == "version") {
                declaration.setVersion(value);
            } else if (name == "encoding") {
                declaration.setEncoding(value);
            } else if (name == "standalone") {
                if (value == "yes") {
                    declaration.setStandalone(Declaration::Standalone::Yes);
                } else if (value == "no") {
                    declaration.setStandalone(Declaration::Standalone::No);
                } else {
                    NGREST_XML_THROW("Invalid Attribute value: " + value, fileName, line);
                }
            } else {
                NGREST_XML_THROW("Invalid Attribute name: " + name, fileName, line);
            }
        }
    }

    void readElement(Element& element)
    {
        NGREST_XML_ASSERT(test("<"), "Element start expected", "", line);

        char ch = '\0';
        std::string name;
        std::string value;
        readStringBeforeChr(name);
        NGREST_XML_ASSERT(validateId(name), "Element name validation failed: [" + name + "]",
                          fileName, line);
        element.setName(name);

        skipWs();

        // read attributes and namespaces
        while (!test(">")) {
            // element without children
            if (test("/>"))
                return;

            readStringBeforeChr(name, " \n\r\t=");
            skipWs();
            NGREST_XML_ASSERT(test("="), "'=' expected while reading attribute/namespace [" + name + "]",
                              fileName, line);
            skipWs();

            readChar(ch);
            NGREST_XML_ASSERT(ch == '"' || ch == '\'',
                              "quote expected while reading attribute/namespace ["
                              + name + "]", fileName, line);
            readStringWithChr(value, ch);
            unescapeString(value);

            if (name == "xmlns") {
                // default namespace
                element.declareDefaultNamespace(value);
            } else if (!name.compare(0, 6, "xmlns:")) {
                // namespace
                element.declareNamespace(value, name.substr(6));
            } else {
                // attribute
                element.createAttribute(name, value);
            }

            skipWs();
        }

        // read children

        for (;;) {
            // text node child
            readStringWithChr(value, '<');
            unescapeString(value);

            if (!value.empty()) {
                if (hasText(value) || !stripWhitespace)
                    element.createText(value);
            }

            // some node

            // comment
            if (test("!--")) {
                ReadStringWithStr(value, "-->");
                element.createComment(value);
            } else if (test("![CDATA[")) {
                // cdata
                ReadStringWithStr(value, "]]>");
                element.createCdata(value);
            } else if (test("/")) {
                // end of parent element
                readStringBeforeChr(name);
                NGREST_XML_ASSERT(name == element.getPrefixName(),
                                  "Invalid name of element end."
                                  " Found: [" + name + "] expected [" + element.getPrefixName() + "]",
                                  fileName, line);
                skipWs();
                NGREST_XML_ASSERT(test(">"), "'>' Expected", fileName, line);
                break;
            } else {
                // child element
                // moving back to the '<' char
                stream.seekg(static_cast<std::istream::off_type>(-1), std::ios::cur);
                readElement(element.createElement());
            }
        }
    }


    void unescapeString(std::string& str)
    {
        std::string::size_type posStart = 0;
        std::string::size_type posEnd = 0;
        while ((posStart = str.find_first_of('&', posEnd)) != std::string::npos) {
            posEnd = str.find_first_of(';', posStart);
            NGREST_XML_ASSERT(posEnd != std::string::npos, "';' not found while unescaping string ["
                              + str + "]", fileName, line);

            const std::string& esc = str.substr(posStart + 1, posEnd - posStart - 1);
            NGREST_XML_ASSERT(!esc.empty(), "Invalid sequence found while unescaping string ["
                              + str + "]", fileName, line);

            if (esc == "lt") {
                str.replace(posStart, posEnd - posStart + 1, 1, '<');
                posEnd = posStart + 1;
            } else if (esc == "gt") {
                str.replace(posStart, posEnd - posStart + 1, 1, '>');
                posEnd = posStart + 1;
            } else if (esc == "amp") {
                str.replace(posStart, posEnd - posStart + 1, 1, '&');
                posEnd = posStart + 1;
            } else if (esc == "apos") {
                str.replace(posStart, posEnd - posStart + 1, 1, '\'');
                posEnd = posStart + 1;
            } else if (esc == "quot") {
                str.replace(posStart, posEnd - posStart + 1, 1, '"');
                posEnd = posStart + 1;
            } // utf-16 escaping: leave it as is
        }
    }


    bool readChar(char& ch)
    {
        stream.get(ch);
        if (ch == '\n')
            ++line;

        return true;
    }

    char readChar()
    {
        char ch = '\0';
        stream.get(ch);
        if (ch == '\n')
            ++line;
        return ch;
    }

    void skipWs()
    {
        char ch = '\0';
        static const std::string whitespace = " \t\n\r";

        for (;;) {
            ch = stream.peek();
            if (whitespace.find(ch) == std::string::npos)
                break;

            if (ch == '\n')
                ++line;
            stream.ignore();
        }
    }

    void readStringBeforeChr(std::string& str, const std::string& charset =
            " \t\n\r!\"#$%&\'()*+,/;<=>?@[\\]^`{|}~")
    {
        char ch = '\0';
        str.erase();
        for (;;) {
            ch = stream.peek();
            if (charset.find(ch) != std::string::npos)
                break;

            if (ch == '\n')
                ++line;
            str.append(1, ch);
            stream.ignore();
        }
    }

    void readStringWithChr(std::string& str, const char end)
    {
        char ch = '\0';
        str.erase();
        for (;;) {
            ch = stream.peek();
            if (end == ch) {
                stream.ignore();
                break;
            }

            if (ch == '\n')
                ++line;

            str.append(1, ch);
            stream.ignore();
        }
    }

    void ReadStringWithStr(std::string& str, const char* end)
    {
        str.erase();
        char ch = '\0';
        const char* curr = nullptr;

        for (;;) {
            curr = end;
            for (;;) {
                readChar(ch);
                if (ch == *curr)
                    break;

                str.append(1, ch);
            }

            // first matched char found
            for (;;) {
                ++curr;
                if (!*curr) // found
                    return;

                // read and compare next char
                readChar(ch);
                if (ch != *curr) {
                    // adding matched chars
                    str.append(end, curr - end);
                    // continuing search
                    break;
                }
            }
        }
    }

    bool test(const char* str)
    {
        char ch = '\0';
        const char* curr = str;
        unsigned lines = 0;
        for (; *curr; ++curr) {
            ch = stream.peek();

            if (ch != *curr) {
                if (curr != str)
                    stream.seekg(static_cast<std::istream::off_type>(str - curr), std::ios::cur);
                return false;
            }

            stream.ignore();
            if (ch == '\n')
                ++lines;
        }

        line += lines;

        return true;
    }

    bool hasText(const std::string& str)
    {
        static const char* whitespace = " \t\n\r";
        return str.find_first_not_of(whitespace) != std::string::npos;
    }

    bool validateId(const std::string& id)
    {
        static const std::string nonId = " \t\n\r!\"#$%&\'()*+,/;<=>?@[\\]^`{|}~";

        if (id.empty())
            return false;

        if (id.find_first_of(nonId) != std::string::npos)
            return false;

        const char first = id[0];
        if (first == '.' || first == '-' || (first >= '0' && first <= '9'))
            return false;

        return true;
    }

public:
    std::istream& stream;
    std::string fileName;
    std::string encoding;
    bool stripWhitespace;
    unsigned line;
};


XmlReader::XmlReader(std::istream& stream, bool stripWhitespace):
    impl(new XmlReaderImpl(stream, stripWhitespace))
{
}

XmlReader::~XmlReader()
{
    delete impl;
}

void XmlReader::setEncoding(const std::string& encoding)
{
    impl->encoding = encoding;
}

void XmlReader::setFileName(const std::string& fileName)
{
    impl->fileName = fileName;
}

void XmlReader::readDocument(Document& document)
{
    try {
        impl->line = 0;
        impl->stream.exceptions(std::ios::failbit | std::ios::badbit);
        impl->line = 1;
        impl->readDeclaration(document.getDeclaration());
        if (!impl->encoding.empty()) {
            document.getDeclaration().setEncoding(impl->encoding);
        } else {
            impl->encoding = document.getDeclaration().getEncoding();
        }
        impl->skipWs();
        std::string comment;
        bool skip = false;
        // skip comments before root element node
        do {
            skip = false;
            if (impl->test("<!--")) {
                impl->ReadStringWithStr(comment, "-->");
                skip = true;
            }
            impl->skipWs();
            if (impl->test("<?")) {
                impl->ReadStringWithStr(comment, "?>");
                skip = true;
            }
            impl->skipWs();
        }
        while (skip);
        impl->readElement(document.getRootElement());
    }
    catch (const Exception&)
    {
        throw;
    }
    catch (const std::exception& exception)
    {
        NGREST_XML_THROW("Error while parsing file: " + Error::getLastError() + "(" +
                         std::string(exception.what()) + ")", impl->fileName, impl->line);
    }
}

void XmlReader::readDeclaration(Declaration& declaration)
{
    impl->readDeclaration(declaration);
}

void XmlReader::readElement(Element& element)
{
    impl->readElement(element);
}


XmlFileReader::XmlFileReader(const std::string& fileName, bool stripWhitespace):
    XmlReader(stream, stripWhitespace)
{
    setFileName(fileName);
    stream.open(fileName.c_str(), std::ios::binary);
}

} // namespace xml
} // namespace ngrest
