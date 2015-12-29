#include "Exception.h"
#include "Document.h"
#include "Declaration.h"
#include "Attribute.h"
#include "Namespace.h"
#include "Node.h"
#include "Element.h"
#include "XmlWriter.h"

namespace ngrest {
namespace xml {

XmlWriter::XmlWriter(std::ostream& stream, bool enableFormatting):
    stream(stream), enableFormatting(enableFormatting), indent(0)
{
}

void XmlWriter::writeDocument(const Document& document)
{
    stream.exceptions(std::ios::failbit | std::ios::badbit);
    writeDeclaration(document.getDeclaration());
    writeElement(document.getRootElement());
}

void XmlWriter::writeDeclaration(const Declaration& declaration)
{
    stream << "<?xml";
    writeAttribute(Attribute("version", declaration.getVersion()));

    if (!declaration.getEncoding().empty())
        writeAttribute(Attribute("encoding", declaration.getEncoding()));

    if (declaration.getStandalone() == Declaration::Standalone::Yes) {
        writeAttribute(Attribute("standalone", "yes"));
    } else if (declaration.getStandalone() == Declaration::Standalone::No) {
        writeAttribute(Attribute("standalone", "no"));
    }

    stream << " ?>";
}

void XmlWriter::writeNode(const Node& node)
{
    switch (node.getType()) {
    case Node::Type::Element:
        writeElement(node.getElement());
        break;

    case Node::Type::Text:
        writeText(static_cast<const Text&>(node));
        break;

    case Node::Type::Comment:
        writeComment(static_cast<const Comment&>(node));
        break;

    case Node::Type::Cdata:
        writeCdata(static_cast<const Cdata&>(node));
        break;

    default:
        NGREST_THROW_ASSERT("Invalid Node Type: " + toString(static_cast<int>(node.getType())));
    }
}

void XmlWriter::writeElement(const Element& element)
{
    writeIndent();
    stream << "<" << element.getPrefixName();

    // write namespaces
    for (const Namespace* ns = element.getFirstNamespace(); ns; ns = ns->getNextSibling())
        writeNamespace(*ns);

    // write attributes
    for (const Attribute* attribute = element.getFirstAttribute();
         attribute; attribute = attribute->getNextSibling())
        writeAttribute(*attribute);

    if (element.isEmpty()) {
        // end element
        stream << "/>";
    } else {
        stream << ">";

        if (element.isLeaf()) {
            writeText(static_cast<const Text&>(*element.getFirstChild()));
        } else {
            // write children

            ++indent;
            for (const Node* node = element.getFirstChild(); node; node = node->getNextSibling())
                writeNode(*node);
            --indent;
            writeIndent();
        }
        stream << "</" << element.getPrefixName() << ">";
    }
}

void XmlWriter::writeComment(const Comment& comment)
{
    writeIndent();
    stream << "<!--" << escapeString(comment.getValue()) << "-->";
}

void XmlWriter::writeText(const Text& text)
{
    stream << escapeString(text.getValue());
}

void XmlWriter::writeCdata(const Cdata& cdata)
{
    writeIndent();
    std::string str(cdata.getValue());
    std::string::size_type pos = 0;
    while ((pos = str.find("]]>", pos)) != std::string::npos) {
        str.replace(pos, 3, "]]]]><![CDATA[>", 15);
        pos += 15;
    }

    stream << "<![CDATA[" << str << "]]>";
}

void XmlWriter::writeAttribute(const Attribute& attribute)
{
    stream << " " << attribute.getPrefixName() << "=\"" << escapeString(attribute.getValue()) << "\"";
}

void XmlWriter::writeNamespace(const Namespace& ns)
{
    if (ns.getPrefix().empty()) {
        stream << " xmlns";
    } else {
        stream << " xmlns:" << ns.getPrefix();
    }

    stream << "=\"" << escapeString(ns.getUri()) << "\"";
}

std::string XmlWriter::escapeString(std::string str)
{
    static const char* szEscapeChars = "<>&'\"";

    std::string::size_type pos = 0;
    while ((pos = str.find_first_of(szEscapeChars, pos)) != std::string::npos) {
        switch (str[pos]) {
        case '<':
            str.replace(pos, 1, "&lt;", 4);
            pos += 4;
            break;

        case '>':
            str.replace(pos, 1, "&gt;", 4);
            pos += 4;
            break;

        case '&':
            str.replace(pos, 1, "&amp;", 5);
            pos += 5;
            break;

        case '\'':
            str.replace(pos, 1, "&apos;", 6);
            pos += 6;
            break;

        case '"':
            str.replace(pos, 1, "&quot;", 6);
            pos += 6;
            break;
        }
    }

    return str;
}

void XmlWriter::writeIndent()
{
    if (enableFormatting) {
        stream << "\n";
        for (unsigned i = indent; i; --i)
            stream << "  ";
    }
}

void XmlWriter::writeNewLine()
{
    if (enableFormatting)
        stream << "\n";
}


XmlFileWriter::XmlFileWriter(const std::string& fileName, bool enableFormatting):
    XmlWriter(stream, enableFormatting)
{
    stream.open(fileName.c_str());
}

} // namespace xml
} // namespace ngrest
