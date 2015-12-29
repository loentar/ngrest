#ifndef NGREST_XML_EXCEPTION_H
#define NGREST_XML_EXCEPTION_H

#include <ngrest/utils/Exception.h>
#include <ngrest/utils/tostring.h>
#include "ngrestxmlexport.h"

//! throw xml exception
#define NGREST_XML_THROW(DESCRIPTION, XMLFILE, XMLLINE) \
    throw ::ngrest::xml::XmlException(NGREST_FILE_LINE, __FUNCTION__, DESCRIPTION, XMLFILE, XMLLINE);

//! assert expression
#define NGREST_XML_ASSERT(EXPRESSION, DESCRIPTION, XMLFILE, XMLLINE) \
    if (!(EXPRESSION)) NGREST_XML_THROW(DESCRIPTION, XMLFILE, XMLLINE)

namespace ngrest {
namespace xml {

//! ngrest exception
class XmlException: public ::ngrest::Exception
{
public:
    //! exception constructor
    /*! \param  fileLine - source file name and line number
        \param  function - function signature
        \param  description - description
        \param  xmlFile - xml file name where error occured
        \param  xmlLine - xml file line where error occured
      */
    inline XmlException(const char* fileLine, const char* function, std::string description,
                        const std::string& xmlFile, int xmlLine):
        Exception(fileLine, function, description.append(": ")
                  .append(xmlFile).append(":").append(toString(xmlLine)))
    {
    }
};

} // namespace xml
} // namespace ngrest

#endif // #ifndef NGREST_XML_EXCEPTION_H
