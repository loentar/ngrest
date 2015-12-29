#ifndef NGREST_XML_DECLARATION_H
#define NGREST_XML_DECLARATION_H

#include <string>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

//! xml-document declaration
class NGREST_XML_EXPORT Declaration
{
public:
    enum class Standalone //! standalone
    {
        Undefined, //!< not defined
        No,        //!< standalone="no"
        Yes        //!< standalone="yes"
    };

public:
    //! constructor
    Declaration();

    //! destructor
    ~Declaration();

    //! get version
    /*! \return version
      */
    const std::string& getVersion() const;

    //! set version
    /*! \param version - version
      */
    void setVersion(const std::string& version);


    //! get encoding
    /*! \return encoding
      */
    const std::string& getEncoding() const;

    //! set encoding
    /*! \param encoding - encoding
      */
    void setEncoding(const std::string& encoding);


    //! get stand alone
    /*! \return stand alone
      */
    Standalone getStandalone() const;

    //! set stand alone
    /*! \param standalone - standalone
      */
    void setStandalone(Standalone standalone);


private:
    std::string  version;      //!< xml document version(1.0)
    std::string  encoding;     //!< encoding
    Standalone   standalone;   //!< standalone declaration
};

} // namespace xml
} // namespace ngrest

#endif // NGREST_XML_DECLARATION_H

