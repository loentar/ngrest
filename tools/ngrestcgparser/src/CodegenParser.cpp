#include <iostream>
#include "CodegenParser.h"

namespace ngrest {
namespace codegen {

ParseSettings::ParseSettings():
    noServiceWarn(false)
{
}

CodegenParser::~CodegenParser()
{
}

ParseException::ParseException(const std::string& file_, int line_, const std::string& message_,
                               const std::string& sourceFile_, int sourceLine_):
    file(file_), line(line_), message(message_),
    sourceFile(sourceFile_), sourceLine(sourceLine_)
{
}

std::ostream& ParseException::operator<<(std::ostream& stream) const
{
    return stream << file << "[" << line << "]: " << message
                  << "\n While parsing " << sourceFile << "[" << sourceLine << "]\n";
}

std::string& ParseException::getMessage()
{
    return message;
}


std::ostream& operator<<(std::ostream& stream, const ParseException& parseException)
{
    return parseException.operator<<(stream);
}

}
}
