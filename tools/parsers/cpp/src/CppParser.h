#ifndef _CPPPARSER_H_
#define _CPPPARSER_H_

#include <ngrest/codegen/CodegenParser.h>

namespace ngrest {
namespace codegen {

class CppParser: public CodegenParser
{
public:
    virtual const std::string& getId();
    virtual void process(const ParseSettings& parseSettings, Project& project);

private:
    static const std::string id;
};

}
}

#endif // _CPPPARSER_H_
