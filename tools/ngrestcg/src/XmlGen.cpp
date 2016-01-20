#include <list>
#include <ngrest/utils/stringutils.h>
#include <ngrest/utils/tostring.h>
#include <ngrest/utils/Log.h>
#include <ngrest/utils/Exception.h>
#include <ngrest/xml/Document.h>
#include <ngrest/xml/Element.h>
#include <ngrest/codegen/Interface.h>
#include "XmlGen.h"

namespace ngrest {
namespace codegen {

std::string typeToString(const DataType::Type type)
{
    switch (type) {
    case DataType::Type::Generic:
        return "generic";

    case DataType::Type::String:
        return "string";

    case DataType::Type::DataObject:
        return "dataobject";

    case DataType::Type::Enum:
        return "enum";

    case DataType::Type::Struct:
        return "struct";

    case DataType::Type::Typedef:
        return "typedef";

    case DataType::Type::Template:
        return "template";

    default:
        return "unknown";
    }
}


template<typename Type>
xml::Element& operator<<(xml::Element& element, const std::list<Type>& list)
{
    for(typename std::list<Type>::const_iterator it = list.begin(); it != list.end(); ++it)
        element << *it;
    return element;
}

bool dataTypeToString(std::string& out, const DataType& dataType, bool asUsed = false,
                 bool noModifiers = false)
{
    if (!noModifiers && dataType.isConst)
        out += "const ";

    if (!noModifiers && !dataType.prefix.empty())
        out += dataType.prefix + " ";

    if (asUsed && !dataType.usedName.empty()) {
        out += dataType.usedName;
    } else {
        out += dataType.ns;
        if (!dataType.ownerName.empty())
            out += dataType.ownerName + "::";
        out += dataType.name;
    }

    bool bIsTemplate = !dataType.params.empty();
    if (!bIsTemplate) {
        if (!noModifiers) {
            out += (dataType.isRef ? "&" : "");
        }
        return false;
    }

    out += "<";
    std::string::size_type beginTemplatePos = out.size();
    bool space = !dataType.params.back().params.empty();
    bool first = true;

    for (std::list<DataType>::const_iterator it = dataType.params.begin();
         it != dataType.params.end(); ++it) {
        if (!first)
            out += ", ";
        bIsTemplate = dataTypeToString(out, *it, asUsed);
        if (first) {
            if (out.substr(beginTemplatePos, 2) == "::")
                space = true;
            first = false;
        }
    }
    if (space) { // first parameter begins with :: or last parameter was template: insert spaces
        out.insert(beginTemplatePos, " ");
        out += ' ';
    }
    out += ">";

    if (!noModifiers)
        out += (dataType.isRef ? "&" : "");
    return true;
}

xml::Element& operator<<(xml::Element& elemDataTypes, const DataType& dataType)
{
    std::string usedTypedef;
    dataTypeToString(usedTypedef, dataType, true);

    std::string nsName;
    dataTypeToString(nsName, dataType, false, true);

    std::string type;
    dataTypeToString(type, dataType);
    elemDataTypes.setValue(type);

    elemDataTypes.createElement("isConst", dataType.isConst);
    elemDataTypes.createElement("isRef", dataType.isRef);
    elemDataTypes.createElement("usedName", !dataType.usedName.empty() ? dataType.usedName : usedTypedef);
    elemDataTypes.createElement("name", dataType.name);
    elemDataTypes.createElement("nsName", nsName);
    elemDataTypes.createElement("ns", dataType.ns);
    elemDataTypes.createElement("type", typeToString(dataType.type));
    elemDataTypes.createElement("usedTypedef", usedTypedef);

    xml::Element& elemTemplateParams = elemDataTypes.createElement("templateParams");
    int num = 1;
    for(std::list<DataType>::const_iterator it = dataType.params.begin();
        it != dataType.params.end(); ++it, ++num)
        elemTemplateParams.createElement("templateParam" + toString(num)) << *it;

    if (dataType.type == DataType::Type::Unknown)
        LogWarning() << "Unknown datatype: " << (dataType.ns + dataType.name);

    return elemDataTypes;
}

xml::Element& operator<<(xml::Element& elemParams, const Param& param)
{
    if (param.name.size() != 0 && param.dataType.name != "void") {
        xml::Element& elemParam = elemParams.createElement("param", "");

        elemParam.createElement("name", param.name);
        elemParam.createElement("description", param.description);
        elemParam.createElement("details", param.details);

        xml::Element& elemOptions = elemParam.createElement("options");
        for (StringMap::const_iterator itOption = param.options.begin();
             itOption != param.options.end(); ++itOption)
            elemOptions.createElement(itOption->first, itOption->second);
        elemParam.createElement("dataType") << param.dataType;

        std::string value = elemParams.getValue();
        if (!value.empty())
            value += ", ";

        dataTypeToString(value, param.dataType, true);
        value += " " + param.name;
        elemParams.setValue(value);
    }

    return elemParams;
}

xml::Element& operator<<(xml::Element& elemOperations, const Operation& operation)
{
    xml::Element& elemOperation = elemOperations.createElement("operation");

    elemOperation.createElement("name", operation.name);
    elemOperation.createElement("description", operation.description);
    elemOperation.createElement("details", operation.details);
    elemOperation.createElement("isConst", operation.isConst);
    elemOperation.createElement("const", operation.isConst ? " const" : "");
    elemOperation.createElement("isAsynch", operation.isAsynch);

    xml::Element& elemOptions = elemOperation.createElement("options");

    for (StringMap::const_iterator itOption = operation.options.begin();
         itOption != operation.options.end(); ++itOption)
        elemOptions.createElement(itOption->first, itOption->second);

    elemOperation.createElement("params", "") << operation.params;

    xml::Element& elemReturn = elemOperation.createElement("return");
    elemReturn << operation.returnType.dataType;
    elemReturn.createElement("responseName", operation.returnType.name);

    return elemOperations;
}


xml::Element& operator<<(xml::Element& elemEnumMembers, const Enum::Member& member)
{
    xml::Element& elemOperation = elemEnumMembers.createElement("member");

    elemOperation.createElement("name", member.name);
    elemOperation.createElement("value", member.value);
    elemOperation.createElement("description", member.description);

    return elemEnumMembers;
}

void writeCppNs(xml::Element& node, const std::string& ns)
{
    const std::string& realNs = (ns.substr(0, 2) == "::") ? ns.substr(2) : ns;

    std::string startNs;
    std::string endNs;
    std::string::size_type pos = 0;
    std::string::size_type prevPos = 0;
    while((pos = realNs.find("::", pos)) != std::string::npos) {
        startNs += "namespace " + realNs.substr(prevPos, pos - prevPos) + " {\n";
        endNs += "}\n";
        pos += 2;
        prevPos = pos;
    }

    node.createElement("startCppNs", startNs);
    node.createElement("endCppNs", endNs);
}

xml::Element& operator<<(xml::Element& elemServices, const Service& service)
{
    xml::Element& elemService = elemServices.createElement("service");

    std::string serviceNs = (service.ns.substr(0, 2) == "::") ?
                service.ns.substr(2) : service.ns;
    stringReplace(serviceNs, "::", ".", true);

    elemService.createElement("name", service.name);
    elemService.createElement("ns", service.ns);
    elemService.createElement("nsName", service.ns + service.name);
    elemService.createElement("serviceNs", serviceNs);
    elemService.createElement("serviceNsName", serviceNs + service.name);
    elemService.createElement("description", service.description);
    elemService.createElement("details", service.details);

    xml::Element& elemOptions = elemService.createElement("options");
    for (StringMap::const_iterator itOption = service.options.begin();
         itOption != service.options.end(); ++itOption)
        elemOptions.createElement(itOption->first, itOption->second);

    xml::Element& elemModules = elemService.createElement("modules");
    for (StringList::const_iterator itModule = service.modules.begin();
         itModule != service.modules.end(); ++itModule)
        elemModules.createElement("module", *itModule);

    elemService.createElement("operations") << service.operations;

    writeCppNs(elemService, service.ns);

    return elemServices;
}

xml::Element& operator<<(xml::Element& elemEnums, const Enum& en)
{
    NGREST_ASSERT(!en.isForward || en.isExtern, "Enum \"" + en.name + "\" is not fully declared");

    xml::Element& elemEnum = elemEnums.createElement("enum");

    elemEnum.createElement("name", en.name);
    elemEnum.createElement("nsName", en.ns +
                          (en.ownerName.empty() ? "" : (en.ownerName + "::")) + en.name);
    elemEnum.createElement("ns", en.ns);
    elemEnum.createElement("owner", en.ownerName);
    elemEnum.createElement("description", en.description);
    elemEnum.createElement("details", en.details);
    elemEnum.createElement("isExtern", en.isExtern);
    elemEnum.createElement("members") << en.members;

    writeCppNs(elemEnum, en.ns);

    xml::Element& elemOptions = elemEnum.createElement("options");

    for (StringMap::const_iterator itOption = en.options.begin();
         itOption != en.options.end(); ++itOption)
        elemOptions.createElement(itOption->first, itOption->second);

    return elemEnums;
}

xml::Element& operator<<(xml::Element& elemStructs, const Struct& structure)
{
    NGREST_ASSERT(!structure.isForward || structure.isExtern,
                  "Struct \"" + structure.name + "\" is not fully declared");

    xml::Element& elemStruct = elemStructs.createElement("struct");

    elemStruct.createElement("name", structure.name);
    elemStruct.createElement("nsName", structure.ns +
                            (structure.ownerName.empty() ? "" : (structure.ownerName + "::")) +
                            structure.name);
    elemStruct.createElement("ns", structure.ns);
    elemStruct.createElement("owner", structure.ownerName);

    // parent
    std::string::size_type pos = structure.parentName.find_last_of("::");
    const std::string parentName = (pos != std::string::npos) ?
                structure.parentName.substr(pos + 1) : structure.parentName;
    elemStruct.createElement("parentName", parentName);
    elemStruct.createElement("parentUsedName", structure.parentName);
    elemStruct.createElement("parentNs", structure.parentNs);
    elemStruct.createElement("parentNsName", structure.parentNs + parentName);
    elemStruct.createElement("description", structure.description);
    elemStruct.createElement("details", structure.details);
    elemStruct.createElement("isExtern", structure.isExtern);
    elemStruct.createElement("fields") << structure.fields;

    writeCppNs(elemStruct, structure.ns);

    xml::Element& elemOptions = elemStruct.createElement("options");
    for (StringMap::const_iterator itOption = structure.options.begin();
         itOption != structure.options.end(); ++itOption)
        elemOptions.createElement(itOption->first, itOption->second);

    elemStruct.createElement("enums") << structure.enums;

    elemStruct.createElement("structs") << structure.structs;

    return elemStructs;
}

xml::Element& operator<<(xml::Element& elemTypedefs, const Typedef& td)
{
    xml::Element& elemTypedef = elemTypedefs.createElement("typedef");

    elemTypedef.createElement("name", td.name);
    elemTypedef.createElement("nsName", td.ns + td.name);
    elemTypedef.createElement("ns", td.ns);
    elemTypedef.createElement("description", td.description);
    elemTypedef.createElement("details", td.details);
    elemTypedef.createElement("dataType") << td.dataType;
    elemTypedef.createElement("isExtern", td.isExtern);

    writeCppNs(elemTypedef, td.ns);

    xml::Element& elemOptions = elemTypedef.createElement("options");

    for (StringMap::const_iterator itOption = td.options.begin();
         itOption != td.options.end(); ++itOption) {
        elemOptions.createElement(itOption->first, itOption->second);
    }

    return elemTypedefs;
}

xml::Element& operator<<(xml::Element& elemInterfaces, const Interface& interface)
{
    xml::Element& elemInterface = elemInterfaces.createElement("interface");

    elemInterface.createElement("name", interface.name);
    elemInterface.createElement("ns", interface.ns);
    elemInterface.createElement("nsName", interface.ns + interface.name);
    elemInterface.createElement("fileName", interface.fileName);
    elemInterface.createElement("filePath", interface.filePath);

    xml::Element& elemOptions = elemInterface.createElement("options");
    for (StringMap::const_iterator itOption = interface.options.begin();
         itOption != interface.options.end(); ++itOption) {
        elemOptions.createElement(itOption->first, itOption->second);
    }

    // included files
    xml::Element& elemIncludes = elemInterface.createElement("includes");
    for (std::list<Include>::const_iterator itInclude = interface.includes.begin();
         itInclude != interface.includes.end(); ++itInclude) {
        std::string ns = itInclude->ns.substr(0, 2) == "::" ? itInclude->ns.substr(2) : itInclude->ns;
        stringReplace(ns, "::", ".", true);

        xml::Element& elemInclude = elemIncludes.createElement("include");
        elemInclude.createElement("name", itInclude->interfaceName);
        elemInclude.createElement("ns", itInclude->ns);
        elemInclude.createElement("nsName", ns + itInclude->interfaceName);
        elemInclude.createElement("fileName", itInclude->fileName);
        elemInclude.createElement("filePath", itInclude->filePath);
//        elemInclude.createElement("targetNs", itInclude->targetNs);
    }

    elemInterface.createElement("enums") << interface.enums;
    elemInterface.createElement("structs") << interface.structs;
    elemInterface.createElement("typedefs") << interface.typedefs;
    elemInterface.createElement("services") << interface.services;

    return elemInterfaces;
}

xml::Element& operator<<(xml::Element& rootNode, const Project& project)
{
    rootNode.setName("project");
    rootNode.createElement("name", project.name);
    rootNode.createElement("ns", project.ns);
    rootNode.createElement("nsName", project.ns + project.name);
    writeCppNs(rootNode, project.ns);

    rootNode.createElement("interfaces") << project.interfaces;

    return rootNode;
}

}
}
