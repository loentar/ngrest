#ifndef NGREST_XMLGEN_H
#define NGREST_XMLGEN_H


namespace ngrest {

namespace xml {
class Element;
}

namespace codegen {

struct Project;

//! process project struct into xml
/*! \param  rootNode - resulting xml-project node
      \param  project - project
      \return resulting xml-project node
      */
xml::Element& operator<<(xml::Element& rootNode, const Project& project);
}
}

#endif // NGREST_XMLGEN_H
