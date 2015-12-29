#ifndef NGREST_XML_DOCUMENT_H
#define NGREST_XML_DOCUMENT_H

#include "Declaration.h"
#include "Element.h"
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

//! XML Document
class NGREST_XML_EXPORT Document
{
public:
    //! get document declaration
    /*! \return document declaration
        */
    const Declaration& getDeclaration() const;

    //! get document declaration
    /*! \return document declaration
        */
    Declaration& getDeclaration();

    //! get root Element
    /*! \return root Element
        */
    const Element& getRootElement() const;

    //! get root node
    /*! \return root node
        */
    Element& getRootElement();

private:
    Declaration declaration;  //!< xml-declaration
    Element rootElement;      //!< root element
};

} // namespace xml
} // namespace ngrest


#endif // NGREST_XML_DOCUMENT_H
