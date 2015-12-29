#ifndef NGREST_CODEGENPARSER_H
#define NGREST_CODEGENPARSER_H

#include <string>
#include <iosfwd>
#include "ngrestcodegenparserexport.h"
#include "Interface.h"

namespace ngrest {
namespace codegen {

  //! source code parse settings
  struct NGREST_CODEGENPARSER_EXPORT ParseSettings
  {
    std::string    inDir;          //!<  input dir
    std::string    outDir;         //!<  output dir
    StringList     files;          //!<  input files
    StringMap      env;            //!<  environment - arguments passed through -d option
    bool           noServiceWarn;  //!<  do not dislpay warning if no service found

    ParseSettings();
  };

  //! codegen source code parser plugin
  class NGREST_CODEGENPARSER_EXPORT CodegenParser
  {
  public:
    virtual ~CodegenParser();

    //! get parser's id
    /*! \return parser's id
      */
    virtual const std::string& getId() = 0;

    //! process project
    /*! \param parseSettings - parse settings
        \param project - resulting project
      */
    virtual void process(const ParseSettings& parseSettings, Project& project) = 0;
  };

  //! parse exception
  class NGREST_CODEGENPARSER_EXPORT ParseException
  {
  public:
    ParseException(const std::string& file, int line, const std::string& message,
                    const std::string& sourceFile, int sourceLine);

    std::ostream& operator<<(std::ostream& stream) const;

    std::string& getMessage();

  private:
    std::string file;
    int line;
    std::string message;
    std::string sourceFile;
    int sourceLine;
  };

  NGREST_CODEGENPARSER_EXPORT
  std::ostream& operator<<(std::ostream& stream, const ParseException& parseException);

#define CSP_THROW(CSP_MESSAGE, CSP_FILE, CSP_LINE)\
  throw ::ngrest::codegen::ParseException(__FILE__, __LINE__, CSP_MESSAGE, CSP_FILE, CSP_LINE)

#define CSP_ASSERT(CSP_EXPRESSION, CSP_MESSAGE, CSP_FILE, CSP_LINE)\
  if (!(CSP_EXPRESSION)) CSP_THROW(CSP_MESSAGE, CSP_FILE, CSP_LINE)

}
}

#endif // NGREST_CODEGENPARSER_H
