#ifndef NGREST_CODEGEN_H
#define NGREST_CODEGEN_H

#include <string>
#include <ngrest/codegen/Interface.h>

namespace ngrest {

namespace xml {
class Element;
}

namespace codegen {

//! Code generator
class CodeGen
{
public:
    //! start code generation
    /*! \param  templateDir - path to templates
        \param  outDir - output directory
        \param  rootElement - root element, describing project
        \param  updateOnly - true: update files if needed, false: always update files
        \param  env - environment
        */
    void start(const std::string& templateDir, const std::string& outDir,
               const xml::Element& rootElement, bool updateOnly, const StringMap& env);
};
}
}

#endif // NGREST_CODEGEN_H
