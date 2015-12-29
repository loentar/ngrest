#ifndef NGREST_XML_NAMESPACE_H
#define NGREST_XML_NAMESPACE_H

#include <string>
#include <iosfwd>
#include "ngrestxmlexport.h"

namespace ngrest {
namespace xml {

//! xml namespace
class NGREST_XML_EXPORT Namespace
{
public:
    //! copy constructor
    Namespace(const Namespace& ns);

    //! constructor
    /*! \param prefix - prefix
        \param uri - uri
      */
    Namespace(const std::string& prefix, const std::string& uri);


    //! copy operator
    /*! \param  ns - source namespace
        \return *this
        */
    Namespace& operator=(const Namespace& ns);


    //! get prefix
    /*! \return prefix
      */
    const std::string& getPrefix() const;

    //! set prefix
    /*! \param prefix - prefix
      */
    void setPrefix(const std::string& prefix);


    //! get uri
    /*! \return uri
      */
    const std::string& getUri() const;

    //! set uri
    /*! \param uri - uri
      */
    void setUri(const std::string& uri);


    //! get next sibling namespace
    /*! \return next sibling namespace
      */
    const Namespace* getNextSibling() const;

    //! get next sibling namespace
    /*! \return next sibling namespace
      */
    Namespace* getNextSibling();


    //! check whether the namespaces are equal
    /*! \param  ns - other namespace
        \return true, if namespaces are equal
    */
    bool operator==(const Namespace& ns) const;

    //! check whether the namespaces are not equal
    /*! \param  ns - other namespace
        \return true, if namespaces are not equal
    */
    bool operator!=(const Namespace& ns) const;

private:
    std::string prefix;      //!< prefix
    std::string uri;         //!< uri
    Namespace* nextSibling;  //!< next namespace
    friend class Element;
};

} // namespace xml
} // namespace ngrest

#endif // NGREST_XML_NAMESPACE_H
