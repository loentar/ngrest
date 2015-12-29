#ifndef NGREST_CODEGEN_TOOLS_H
#define NGREST_CODEGEN_TOOLS_H

#include <string>
#include "ngrestcodegenparserexport.h"
#include "Interface.h"

namespace ngrest {
namespace codegen {

//! get base type(currently struct and enum) by full or partial name in current context
/*! \param  nsName - type name in form [[[[some::]namespace::]Struct::]SubStruct::]SubSubstruct
      \param  interface - reference to current interface
      \param  baseType - base type to search
      \param  parent - optional pointer to structure search from
      \return found structure, nullptr if not found
    */
NGREST_CODEGENPARSER_EXPORT
const BaseType* getBaseType(const std::string& nsName, const Interface& interface,
                            const int baseType = BaseType::Type::Any,
                            const Struct* parent = nullptr);

//! get structure by full or partial name in current context
/*! \param  nsName - structure's name in form [[[[some::]namespace::]Struct::]SubStruct::]SubSubstruct
      \param  interface - reference to current interface
      \param  parent - optional pointer to structure search from
      \return found structure, nullptr if not found
    */
NGREST_CODEGENPARSER_EXPORT
const Struct* getStruct(const std::string& nsName, const Interface& interface,
                        const Struct* parent = nullptr);


//! optimize C++ namespace
/*! example:
        optimizeNs: "::samples::ticket::Ticket"
        currentNs: "::samples::sharedtypes::"
        result: "ticket::"

      \param  optimizeNs - namespace to optimize
      \param  currentNs - current namespace
  */
NGREST_CODEGENPARSER_EXPORT
void optimizeCppNs(std::string& optimizeNs, const std::string& currentNs);

//! find existing or add new type in list
/*! \param rList - list
      \param name - type name
      \param namespace - type namespace
      \param ownerName - owner name
      */
template<typename Type>
Type& typeInList(std::list<Type>& list, const std::string& name, const std::string& ns,
                 const std::string& ownerName = "")
{
    typename std::list<Type>::iterator itType = list.begin();
    for (; itType != list.end(); ++itType)
        if (itType->name == name && itType->ns == ns)
            return *itType;

    itType = list.insert(list.end(), Type());
    itType->name = name;
    itType->ns = ns;
    itType->ownerName = ownerName;
    return *itType;
}

//! check and fix identifier
NGREST_CODEGENPARSER_EXPORT
bool fixFileName(std::string& fileName);

//! check and fix identifier
NGREST_CODEGENPARSER_EXPORT
bool fixId(std::string& name, bool bIgnoreBool = false);

//!
NGREST_CODEGENPARSER_EXPORT
const std::string& stringMapValue(const StringMap& map, const std::string& name,
                                  const std::string& defaultValue = "");



}
}

#endif // NGREST_CODEGEN_TOOLS_H
