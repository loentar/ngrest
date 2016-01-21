#ifdef WIN32
#include <io.h>
#include <direct.h>
#include <errno.h>
#else
#include <sys/types.h>
#include <fnmatch.h>
#include <unistd.h>
#include <dirent.h>
#include <errno.h>
#endif
#include <sys/stat.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <list>
#include <map>
#include <stack>
#include <algorithm>
#include <ngrest/utils/File.h>
#include <ngrest/utils/Log.h>
#include <ngrest/utils/tostring.h>
#include <ngrest/utils/fromstring.h>
#include <ngrest/utils/stringutils.h>
#include <ngrest/utils/Exception.h>
#include <ngrest/xml/Element.h>
#include <ngrest/codegen/tools.h>
#include "CodeGen.h"

namespace ngrest {
namespace codegen {

class TemplateParser
{
public:
    TemplateParser():
        line(0), indent(0), isNeedIndent(false), hasConfig(false)
    {
        variables.push(StringMap());
    }

    std::string getElementPath(const xml::Element* element) const
    {
        if (element) {
            std::string path = element->getName();
            while ((element = element->getParent()))
                path = element->getName() + "." + path;
            return path;
        } else {
            return "";
        }
    }

    const xml::Element& getElement(const std::string& variableName, const xml::Element& elem) const
    {
        static xml::Element emptyElement;
        const xml::Element* element = &elem;
        std::string::size_type pos = variableName.find('.');
        std::string variable;
        std::string serviceClass;

        if (pos != std::string::npos) {
            if (variableName.size() == 1)
                // reference to current node
                return elem;

            if (!pos) {
                bool isOpt = false;
                variable = variableName.substr(1);
                NGREST_ASSERT(!variable.empty(), "Element name expected in Name: " + variableName);
                pos = variable.find('.');
                serviceClass = variable.substr(0, pos); // next element name

                if (serviceClass[0] == '*') { // serviceClass can't be empty
                    serviceClass.erase(0, 1);
                    isOpt = true;
                }

                for (; element != nullptr; element = element->getParent()) {
                    const xml::Element* childElement = element->findChildElementByName(serviceClass);
                    if (childElement) {
                        if (pos == std::string::npos) {
                            return *childElement;
                        }

                        variable.erase(0, pos + 1);
                        element = childElement;
                        break;
                    }
                }

                if (!element && isOpt)
                    return emptyElement;
            } else {
                serviceClass = variableName.substr(0, pos);
                variable = variableName.substr(pos + 1);

                while (element != nullptr && element->getName() != serviceClass)
                    element = element->getParent();
            }

            NGREST_ASSERT(element != nullptr, "\nCan't find node which match current class: \"" + serviceClass
                          + "\"\n context: " + getElementPath(&elem) + "\n Variable: " + variableName + "\n");
            NGREST_ASSERT(element->getName() == serviceClass, "\nElement name does not match current class: \""
                          + element->getName() + "\" <=> \"" + serviceClass + "\"\n context: " +
                          getElementPath(&elem) + "\n");

            while ((pos = variable.find('.')) != std::string::npos) {
                const std::string& subClass = variable.substr(0, pos);
                if (subClass[0] == '!' || subClass[0] == '$')
                    break;

                try {
                    if (subClass[0] == '*') {
                        const xml::Element* childElement = element->findChildElementByName(subClass.substr(1));
                        element = childElement ? childElement : &emptyElement;
                    } else {
                        element = &element->getChildElementByName(subClass);
                    }
                } catch(...) {
                    LogError() << "While parsing variable: [" << variableName << "]"
                               << " Element: [" << getElementPath(element) << "]";
                    throw;
                }

                variable.erase(0, pos + 1);
            }
        } else {
            NGREST_ASSERT(element->getName() == variableName, "node name does not match current class: \""
                          + element->getName() + "\" <=> \"" + variableName + "\"\n context: " +
                          getElementPath(&elem) + "\n");
            return elem;
        }

        if (variable[0] == '$') {
            std::string property;
            variable.erase(0, 1);

            pos = variable.find('.');

            if (pos != std::string::npos) {
                property = variable.substr(0, pos);
                variable.erase(0, pos + 1);
            } else {
                property = variable;
                variable.erase();
            }

            // number of this element by order
            if (property == "num") {
                static xml::Element elementNum("num");
                const xml::Element* parentElement = element->getParent();
                NGREST_ASSERT(parentElement != nullptr, "can't get number for node: " + element->getName());

                int num = 0;

                for (const xml::Element* childElement = parentElement->getFirstChildElement();
                     childElement; childElement = childElement->getNextSiblingElement()) {
                    if (childElement == element)
                        break;

                    ++num;
                }

                elementNum.setValue(toString(num));
                element = &elementNum;
            } else if (property == "count") {
                static xml::Element subElementCount("count");
                subElementCount.setValue(toString(element->getChildrenElementsCount()));
                element = &subElementCount;
            } else {
                NGREST_THROW_ASSERT("Unknown Property: [" + variable + "]");
            }

            if (variable.empty())
                return *element;
        }

        if (variable[0] == '!'){
            // exec function
            variable.erase(0, 1);
            element = &executeFunction(variable, *element);
            while (!variable.empty()) {
                // .!trimleft/:/.!dot
                NGREST_ASSERT(variable.substr(0, 2) == ".!", "Junk [" + variable +  "] in variable: ["
                              + variableName + "] at pos " + toString(variableName.size() - variable.size()));
                variable.erase(0, 2);
                element = &executeFunction(variable, *element);
            }
            return *element;
        } else if (variable[0] == '*') {
            // optional node
            const xml::Element* childElement = element->findChildElementByName(variable.substr(1));
            if (childElement) {
                return *childElement;
            } else {
                return emptyElement;
            }
        }

        return element->getChildElementByName(variable);
    }

    std::string::size_type parseParam(std::string& paramBegin) const
    {
        std::string::size_type pos = paramBegin.find_first_of("/\\");
        // slash unescaping
        for (;;) {
            NGREST_ASSERT(pos != std::string::npos, "Can't get param");

            char found = paramBegin[pos];
            if (found == '\\') {
                // unescape
                if (pos < paramBegin.size()) {
                    switch (paramBegin[pos + 1]) {
                    case '/': paramBegin.replace(pos, 2, 1, '/'); break;
                    case 'r': paramBegin.replace(pos, 2, 1, '\r'); break;
                    case 'n': paramBegin.replace(pos, 2, 1, '\n'); break;
                    case 't': paramBegin.replace(pos, 2, 1, '\t'); break;
                    default: paramBegin.erase(pos, 1);
                    }
                }
            } else {
                // '/'
                break;
            }

            pos = paramBegin.find_first_of("/\\", pos + 1);
        }
        return pos;
    }

    const xml::Element& executeFunction(std::string& function, const xml::Element& element) const
    {
        static xml::Element resultElem("result");
        std::string result = element.getTextValue();
        resultElem.clear();

        if (function.substr(0, 9) == "mangledot") {
            stringReplace(result, ".", "_", true);
            function.erase(0, 9);
        } else if (function.substr(0, 6) == "mangle") {
            if (result.size() >= 2 && result.substr(0, 2) == "::")
                result.erase(0, 2);

            if (result.size() >= 2 && result.substr(result.size() - 2, 2) == "::")
                result.erase(result.size() - 2, 2);

            stringReplace(result, "::", "_", true);
            function.erase(0, 6);
        } else if (function.substr(0, 3) == "dot") {
            if (result.size() >= 2 && result.substr(0, 2) == "::")
                result.erase(0, 2);

            if (result.size() >= 2 && result.substr(result.size() - 2, 2) == "::")
                result.erase(result.size() - 2, 2);

            stringReplace(result, "::", ".", true);
            function.erase(0, 3);
        } else if (function.substr(0, 3) == "not") {
            function.erase(0, 3);

            result = result == "true" ? "false" : "true";
        } else if (function.substr(0, 7) == "equals/") {
            function.erase(0, 7);

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            result = (result == what) ? "true" : "false";
        } else if (function.substr(0, 6) == "match/") {
            function.erase(0, 6);

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            result = (result.find(what) != std::string::npos) ? "true" : "false";
        } else if (function.substr(0, 8) == "replace/") {
            function.erase(0, 8);

            // what replace
            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            // replace with
            posEnd = parseParam(function);
            std::string with = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(with, element);

            stringReplace(result, what, with, true);
        } else if (function.substr(0, 13) == "replacenotof/") {
            function.erase(0, 13);

            // what replace
            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            // replace with
            posEnd = parseParam(function);
            std::string with = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(with, element);

            std::string::size_type posStart = 0;
            while ((posEnd = result.find_first_of(what, posStart)) != std::string::npos) {
                result.replace(posStart, posEnd - posStart, with);
                posStart += with.size() + 1;
            }

            if (posStart < result.size())
                result.replace(posStart, result.size() - posStart, with);
        } else if (function.substr(0, 5) == "trim/") {
            function.erase(0, 5);

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            stringTrim(result, what.c_str());
        } else if (function.substr(0, 9) == "trimleft/") {
            function.erase(0, 9);

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            stringTrimLeft(result, what.c_str());
        } else if (function.substr(0, 10) == "trimright/") {
            function.erase(0, 10);

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            stringTrimRight(result, what.c_str());
        } else if (function.substr(0, 4) == "trim") {
            stringTrim(result);
            function.erase(0, 4);
        } else if (function.substr(0, 8) == "trimleft") {
            stringTrimLeft(result);
            function.erase(0, 8);
        } else if (function.substr(0, 7) == "tolower") {
            std::transform(result.begin(), result.end(), result.begin(), ::tolower);
            function.erase(0, 7);
        } else if (function.substr(0, 7) == "toupper") {
            std::transform(result.begin(), result.end(), result.begin(), ::toupper);
            function.erase(0, 7);
        } else if (function.substr(0, 11) == "tocamelcase") {
            function.erase(0, 11);
            std::string::size_type pos = 0;
            while ((pos = result.find('_', pos)) != std::string::npos) {
                if ((pos + 1) < result.size())
                    result.replace(pos, 2, 1, ::toupper(result[pos + 1]));
            }
        } else if (function.substr(0, 9) == "trimright") {
            stringTrimRight(result);
            function.erase(0, 9);
        } else if (function.substr(0, 7) == "append/") {
            function.erase(0, 7);
            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            result += what;
        } else if (function.substr(0, 8) == "prepend/") {
            function.erase(0, 8);
            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            result = what + result;
        } else if (function.substr(0, 9) == "deprefix/") {
            function.erase(0, 9);
            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);


            if (result.substr(0, what.size()) == what)
                result.erase(0, what.size());
        } else if (function.substr(0, 10) == "depostfix/") {
            function.erase(0, 10);
            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            std::string::size_type resSize = result.size();
            std::string::size_type whatSize = what.size();

            if (resSize > whatSize &&
                    result.substr(resSize - whatSize) == what)
                result.erase(resSize - whatSize);
        } else if (function.substr(0, 6) == "token/") {
            function.erase(0, 6);
            const std::string& val = element.getValue();

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            std::string::size_type pos = val.find_first_of(what);

            if (pos != std::string::npos) {
                result = val.substr(0, pos);
            } else {
                result.erase();
            }
        } else if (function.substr(0, 10) == "lasttoken/") {
            function.erase(0, 10);
            const std::string& val = element.getValue();

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            std::string::size_type pos = val.find_last_of(what);

            if (pos != std::string::npos) {
                result = val.substr(pos + 1);
            } else {
                result.erase();
            }
        } else if (function.substr(0, 4) == "cut/") {
            function.erase(0, 4);
            const std::string& val = element.getValue();

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            std::string::size_type pos = val.find_first_of(what);

            if (pos != std::string::npos) {
                result = val.substr(pos + 1);
            } else {
                result.erase();
            }
        } else if (function.substr(0, 8) == "cutlast/") {
            function.erase(0, 8);
            const std::string& val = element.getValue();

            std::string::size_type posEnd = parseParam(function);
            std::string what = function.substr(0, posEnd);
            function.erase(0, posEnd + 1);
            replaceToValue(what, element);

            std::string::size_type pos = val.find_last_of(what);

            if (pos != std::string::npos) {
                result = val.substr(0, pos);
            } else {
                result.erase();
            }
        } else if (function.substr(0, 8) == "fixid") {
            function.erase(0, 8);
            fixId(result);
        } else if (function.substr(0, 3) == "inc") {
            double dTmp = 0;
            fromString(result, dTmp);
            toString(dTmp + 1, result);
            function.erase(0, 3);
        } else if (function.substr(0, 3) == "dec") {
            double dTmp = 0;
            fromString(result, dTmp);
            toString(dTmp - 1, result);
            function.erase(0, 3);
        } else if (function.substr(0, 4) == "add/") {
            std::string::size_type posWhat = function.find('/', 4);

            NGREST_ASSERT(posWhat != std::string::npos, "Can't get operand for add");

            const std::string& what = function.substr(4, posWhat - 4);
            double dOp1 = 0;
            double dOp2 = 0;
            fromString(result, dOp1);
            fromString(what, dOp2);
            toString(dOp1 + dOp2, result);
            function.erase(0, posWhat + 1);
        } else if (function.substr(0, 4) == "sub/") {
            std::string::size_type posWhat = function.find('/', 4);

            NGREST_ASSERT(posWhat != std::string::npos, "Can't get operand for sub");
            const std::string& what = function.substr(4, posWhat - 4);
            double dOp1 = 0;
            double dOp2 = 0;
            fromString(result, dOp1);
            fromString(what, dOp2);
            toString(dOp1 - dOp2, result);
            function.erase(0, posWhat + 1);
        } else if (function.substr(0, 5) == "trunc") {
            double dTmp = 0;
            fromString(result, dTmp);
            toString(static_cast<long>(dTmp), result);
            function.erase(0, 5);
        } else {
            NGREST_THROW_ASSERT("function " + function + " is undefined");
        }

        resultElem.setValue(result);
        return resultElem;
    }


    const std::string& getValue(const std::string& variableName, const xml::Element& element) const
    {
        return getElement(variableName, element).getValue();
    }

    std::string::size_type replaceToValueFindBracketMatch(std::string& str,
                                                          std::string::size_type posStart,
                                                          const xml::Element& element) const
    {
        int recursion = 1;
        std::string::size_type posEnd = posStart;
        while ((posEnd = str.find_first_of("()", posEnd)) != std::string::npos) {
            if (str[posEnd] == ')') {
                if (posEnd > 0 && str[posEnd - 1] == '\\') {
                    ++posEnd;
                    continue;
                }
                --recursion;
                if (recursion == 0)
                    break;
            } else {
                // == '('
                // check for inline $()
                if (str[posEnd - 1] == '$') {
                    std::string::size_type inlineEnd = replaceToValueFindBracketMatch(str, posEnd + 1, element);
                    NGREST_ASSERT(inlineEnd != std::string::npos, "end of inline variable name expected: [" +
                                  str + "]");
                    std::string inl = str.substr(posEnd - 1, inlineEnd - posEnd + 2);
                    replaceToValue(inl, element);
                    str.replace(posEnd - 1, inlineEnd - posEnd + 2, inl);
                    --posEnd; // move to prior the '$('
                    continue;
                }

                ++recursion;
            }
            ++posEnd;
        }
        return posEnd;
    }


    void processValue(const std::string& name, std::string& value, const xml::Element& element) const
    {
        if (name[0] == '$') {
            if (name.substr(0, 16) == "$thisElementName") {
                value = element.getName();
                if (name.size() == 16)
                    return;
                xml::Element valElement(value);
                valElement.setValue(value);
                value = getValue(value + name.substr(16), valElement);
            } else if (name.substr(0, 17) == "$thisElementValue") {
                value = element.getTextValue();
                if (name.size() == 17)
                    return;
                xml::Element valElement(value);
                valElement.setValue(value);
                value = getValue(value + name.substr(17), valElement);
            } else if (name.substr(0, 16) == "$thisElementPath") {
                value = getElementPath(&element);
                if (name.size() == 16)
                    return;
                xml::Element valElement(value);
                valElement.setValue(value);
                value = getValue(value + name.substr(16), valElement);
            } else {
                std::string::size_type pos = name.find('.');
                if (pos != std::string::npos) {
                    // variable + functions
                    const std::string& varName = name.substr(1, pos - 1);
                    value = variables.top()[varName];
                    // process functions and other
                    xml::Element varElement(varName);
                    varElement.setValue(value);
                    value = getValue(name.substr(1), varElement);
                } else {
                    // variable only
                    value = variables.top()[name.substr(1)];
                }
            }
        } else {
            // node value
            value = getValue(name, element);
        }
    }

    void replaceToValue(std::string& str, const xml::Element& element) const
    {
        std::string::size_type posStart = 0;
        std::string::size_type posEnd = 0;

        while((posStart = str.find("$(", posEnd)) != std::string::npos) {
            if (posStart > 0 && str[posStart - 1] == '\\' && posEnd != posStart) {
                str.erase(posStart - 1, 1);
                posEnd = posStart + 1;
                continue;
            }

            posEnd = replaceToValueFindBracketMatch(str, posStart + 2, element);

            NGREST_ASSERT(posEnd != std::string::npos, "end of variable name expected: [" + str + "]");
            const std::string& expression = str.substr(posStart + 2, posEnd - posStart - 2);
            std::string value;

            { // parse "node.name||$var.!func||some.other"
                std::string::size_type namePosBegin = 0;
                std::string::size_type namePosEnd = 0;

                // use first nonempty value
                for(;;) {
                    namePosEnd = expression.find("||", namePosBegin);

                    const std::string& name =
                            expression.substr(namePosBegin, (namePosEnd != std::string::npos)
                                              ? namePosEnd - namePosBegin : namePosEnd);
                    std::string::size_type nameSize = name.size();

                    if (nameSize > 0 && name[0] == '"' && name[nameSize - 1] == '"') {
                        value = name.substr(1, nameSize - 2);
                        replaceToValue(value, element);
                    } else {
                        processValue(name, value, element);
                    }

                    if (!value.empty() || namePosEnd == std::string::npos)
                        break;

                    namePosBegin = namePosEnd + 2;
                }
            }

            str.replace(posStart, posEnd - posStart + 1, value);
            posEnd = posStart + value.size();
        }
    }

    void init(const std::string& inDir)
    {
        hasConfig = File(inDir + NGREST_PATH_SEPARATOR + "codegen.config").isRegularFile();
        if (!hasConfig) {
            StringList files;

            File(inDir).list(files, "*.*", File::AttributeRegularFile);
            for (StringList::const_iterator itFile = files.begin(); itFile != files.end(); ++itFile) {
                if (itFile->find('$') != std::string::npos) {
                    templateFileList.push_back(*itFile);
                } else {
                    constFileList.push_back(*itFile);
                }
            }
        }

        this->inDir = inDir;
    }

    void start(const std::string& outDir, const xml::Element& rootElement, bool updateOnly)
    {
        bool needUpdate = false;

        this->outDir = outDir;

        if (hasConfig) {
            const std::string& inPath = inDir + "codegen.config";
            std::ostringstream outStream;
            std::ifstream inStream;

            inStream.open(inPath.c_str());
            NGREST_ASSERT(inStream.good(), "can't open input file: " + inPath);

            indent = 0;
            line = 0;
            process(inStream, outStream, rootElement);

            inStream.close();
        } else {
            const xml::Element& elementInterfaces = rootElement.getChildElementByName("interfaces");

            for (const xml::Element* childElement = elementInterfaces.getFirstChildElement();
                 childElement; childElement = childElement->getNextSiblingElement()) {
                const xml::Element& elementInterface = *childElement;

                for (std::list<std::string>::const_iterator itTemplateFile = templateFileList.begin();
                     itTemplateFile != templateFileList.end(); ++itTemplateFile) {
                    std::string file = *itTemplateFile;
                    bool isProcessFile = false;
                    try {
                        replaceToValue(file, elementInterface);
                        isProcessFile = true;
                    } catch (const Exception&) {
                        LogVerbose() << "Skipping template file " << file
                                     << " for interface " << elementInterface.getChildElementByName("nsName").getTextValue();
                        continue;
                    }

                    if (isProcessFile) {
                        // erase input path
                        std::string::size_type pos = file.find_last_of(NGREST_PATH_SEPARATOR);
                        if (pos != std::string::npos)
                            file.erase(0, pos + 1);

                        processFile(inDir + *itTemplateFile, outDir, file,
                                    elementInterface, updateOnly, needUpdate);
                    }
                }
            } // for interface

            for (std::list<std::string>::const_iterator itTemplateFile = constFileList.begin();
                 itTemplateFile != constFileList.end(); ++itTemplateFile) {
                std::string file = *itTemplateFile;

                // erase input path
                std::string::size_type pos = file.find_last_of(NGREST_PATH_SEPARATOR);
                if (pos != std::string::npos)
                    file.erase(0, pos + 1);

                processFile(inDir + *itTemplateFile, outDir, file, rootElement, updateOnly, needUpdate);
            }
        } // has config
    }

    void processIfeq(std::istream& in, std::ostream& fsOut, const xml::Element& element, std::string& line,
                     bool isNotEq = false)
    {
        bool isCurrentBlock = true;
        bool isEq = false;
        std::stringbuf data;
        std::string lines;
        int recursion = 1;

        replaceToValue(line, element);

        { //##ifeq(123,321)
            int offsetPos = isNotEq ? 8 : 7;
            std::string::size_type posStart = line.find(",", 7);
            std::string::size_type posEnd = 0;

            NGREST_ASSERT(posStart != std::string::npos, "##ifeq expression is invalid!: \n----\n" + line +
                          "\n----\n");
            posEnd = line.find(')', posStart);
            NGREST_ASSERT(posEnd != std::string::npos, "##ifeq expression is invalid!: \n----\n" + line +
                          "\n----\n");

            std::string left = line.substr(offsetPos, posStart - offsetPos);
            std::string right = line.substr(posStart + 1, posEnd - posStart - 1);

            isEq = false;

            std::string::size_type posStartLeft = 0;
            std::string::size_type posStartRight = 0;
            std::string::size_type posEndLeft = 0;
            std::string::size_type posEndRight = 0;
            do {
                posEndLeft = left.find("||", posStartLeft);
                const std::string& leftCmp = left.substr(posStartLeft, posEndLeft - posStartLeft);

                posStartRight = 0;
                posEndRight = 0;
                do {
                    posEndRight = right.find("||", posStartRight);
                    const std::string& rightCmp = right.substr(posStartRight, posEndRight - posStartRight);

                    if (leftCmp == rightCmp) {
                        isEq = true;
                        break;
                    }
                    posStartRight = posEndRight + 2;
                } while (posEndRight != std::string::npos);
                posStartLeft = posEndLeft + 2;
            } while (posEndLeft != std::string::npos && !isEq);

            if (isNotEq) {
                isEq = !isEq;
            }
        }

        while (!in.eof() && in.good() && recursion > 0) {
            if (in.peek() == '\n') {
                line = "\n";
            } else {
                in.get(data, '\n');
                line = data.str() + "\n";
                data.str("");
            }
            in.ignore();
            in.peek(); // for EOF

            if (line.substr(0, 7) == "##ifeq(" || line.substr(0, 8) == "##ifneq(") {
                ++recursion;
                if (isCurrentBlock == isEq && recursion > 1)
                    lines += line;
            } else if (line.substr(0, 6) == "##else") {
                if (isCurrentBlock == isEq && recursion > 1)
                    lines += line;
                if (recursion == 1)
                    isEq = !isEq;
            } else if (line.substr(0, 7) == "##endif") {
                if (isCurrentBlock == isEq && recursion > 1)
                    lines += line;
                --recursion;
            } else if (isCurrentBlock == isEq) {
                lines += line;
            }
        }

        NGREST_ASSERT(recursion == 0, "Unexpected EOF while parsing: \n---------\n" + lines +
                      "\n------------\n");

        {
            std::istringstream stream(lines);
            process(stream, fsOut, element);
        }
    }

    void processSwitch(std::istream& in, std::ostream& out, const xml::Element& element, std::string& line)
    {
        std::stringbuf data;
        std::string lines;
        int recursion = 1;

        std::string switchExpr = line.substr(9);
        NGREST_ASSERT(!switchExpr.empty(), "switch expression is invalid!");
        stringTrim(switchExpr);
        replaceToValue(switchExpr, element);

        bool equal = false;
        bool processed = false;

        while (!in.eof() && in.good()) {
            if (in.peek() == '\n') {
                line = "\n";
            } else {
                in.get(data, '\n');
                line = data.str();
                if (in.peek() == '\n')
                    line += "\n";
                data.str("");
            }
            in.ignore();
            in.peek(); // for EOF

            if (line.substr(0, 9) == "##switch ") {
                ++recursion;
            } else if (recursion == 1 && !processed && line.substr(0, 7) == "##case ") {
                if (equal) {
                    processed = true;
                    continue;
                }

                std::string caseExpr = line.substr(7);
                stringTrim(caseExpr);

                std::string::size_type casePosStart = 0;
                std::string::size_type casePosEnd = 0;
                do {
                    casePosEnd = caseExpr.find("||", casePosStart);
                    std::string caseExprCmp = caseExpr.substr(casePosStart, casePosEnd - casePosStart);

                    stringTrim(caseExprCmp);
                    replaceToValue(caseExprCmp, element);
                    if (switchExpr == caseExprCmp) {
                        equal = true;
                        break;
                    }

                    casePosStart = casePosEnd + 2;
                } while (casePosEnd != std::string::npos);

                continue;
            } else if (recursion == 1 && !processed && line.substr(0, 9) == "##default") {
                if (equal) {
                    processed = true;
                } else {
                    equal = true;
                }
                continue;
            } else if (line.substr(0, 11) == "##endswitch") {
                --recursion;
                if (recursion == 0) {
                    if (equal)
                        processed = true;
                    break;
                }
            }

            if (equal && !processed)
                lines += line;
        }

        if (equal) {
            std::istringstream stream(lines);
            process(stream, out, element);
        }
    }

    void processForEach(std::istream& in, std::ostream& out, const xml::Element& element, std::string& line)
    {
        std::stringbuf data;
        std::string forEachExpr;
        std::string lines;
        int recursion = 1;

        std::string::size_type posStart = line.find("$(", 9);
        std::string::size_type posEnd = 0;

        NGREST_ASSERT(posStart != std::string::npos, "foreach expression is invalid!");
        posEnd = line.find(')', posStart);
        NGREST_ASSERT(posEnd != std::string::npos, "foreach expression is invalid!");
        forEachExpr = line.substr(posStart + 2, posEnd - posStart - 2);

        while (!in.eof() && in.good()) {
            if (in.peek() == '\n') {
                line = "\n";
            } else {
                in.get(data, '\n');
                line = data.str();
                if (in.peek() == '\n')
                    line += "\n";
                data.str("");
            }
            in.ignore();
            in.peek(); // for EOF

            if (line.substr(0, 10) == "##foreach ") {
                ++recursion;
            } else if (line.substr(0, 8) == "##endfor") {
                --recursion;
                if (recursion == 0)
                    break;
            }

            lines += line;
        }

        NGREST_ASSERT(recursion == 0, "Unexpected EOF while parsing: \n---------\n" + lines +
                      "\n------------\n");

        const xml::Element& subElement = getElement(forEachExpr, element);

        for (const xml::Element* childElement = subElement.getFirstChildElement();
             childElement; childElement = childElement->getNextSiblingElement())
        {
            std::istringstream stream(lines);
            process(stream, out, *childElement);
        }
    }

    void processContext(std::istream& fsIn, std::ostream& fsOut, const xml::Element& element,
                        std::string& line)
    {
        std::stringbuf data;
        std::string contextExpr;
        std::string lines;
        int recursion = 1;

        std::string::size_type posStart = line.find("$(", 9);
        std::string::size_type posEnd = 0;

        NGREST_ASSERT(posStart != std::string::npos, "context expression is invalid!");
        posEnd = line.find(')', posStart);
        NGREST_ASSERT(posEnd != std::string::npos, "context expression is invalid!");
        contextExpr = line.substr(posStart, posEnd - posStart + 1);

        while (!fsIn.eof() && fsIn.good()) {
            if (fsIn.peek() == '\n') {
                line = "\n";
            } else {
                fsIn.get(data, '\n');
                line = data.str();
                if (fsIn.peek() == '\n')
                    line += "\n";
                data.str("");
            }
            fsIn.ignore();
            fsIn.peek(); // for EOF

            if (line.substr(0, 10) == "##context ") {
                ++recursion;
            } else if (line.substr(0, 12) == "##endcontext") {
                --recursion;
                if (recursion == 0)
                    break;
            }

            lines += line;
        }

        NGREST_ASSERT(recursion == 0, "Unexpected EOF while parsing: \n---------\n" + lines +
                      "\n------------\n");

        if (contextExpr[2] == '$') {
            // variable
            replaceToValue(contextExpr, element);
        } else {
            contextExpr.erase(0, 2);
            contextExpr.erase(contextExpr.size() - 1);
        }

        const xml::Element& subElement = getElement(contextExpr, element);

        std::istringstream stream(lines);
        process(stream, fsOut, subElement);
    }

    void processInclude(std::ostream& fsOut, const xml::Element& element, const std::string& line)
    {
        std::string includeFileName;

        char quote = line[0];
        std::string::size_type pos = 0;

        if (quote == '<') {
            pos = line.find('>', 1);
            NGREST_ASSERT(pos != std::string::npos, "include expression is invalid!");
            includeFileName = inDir + "../" + line.substr(1, pos - 1);
        } else if (quote == '"') {
            pos = line.find('"', 1);
            NGREST_ASSERT(pos != std::string::npos, "include expression is invalid!");
            includeFileName = inDir + line.substr(1, pos - 1);
        } else {
            NGREST_THROW_ASSERT("include expression is invalid!");
        }

#ifdef WIN32
        stringReplace(includeFileName, "/", "\\", true);
#endif

        try {
            std::ifstream fsIncFile;
            fsIncFile.open(includeFileName.c_str());

            NGREST_ASSERT(fsIncFile.good(), "can't include file: " + includeFileName);

            std::string currInDir = inDir;
            inDir = includeFileName.substr(0, includeFileName.find_last_of("/\\") + 1);
            while (!fsIncFile.eof() && fsIncFile.good())
                process(fsIncFile, fsOut, element);
            inDir = currInDir;

            fsIncFile.close();
        } catch(...) {
            LogError() << "While processing included file " << includeFileName;
            throw;
        }
    }

    void processIndent(const std::string& line)
    {
        std::string value = line.substr(9);
        stringTrim(value);
        if (value == "+") {
            NGREST_ASSERT(indent < 1024, "Invalid indentation: " + toString(indent + 1)
                          + " while processing line: \n" + line);
            ++indent;
        } else if (value == "-") {
            NGREST_ASSERT(indent > 0, "Invalid indentation: " + toString(indent - 1)
                          + " while processing line: \n" + line);
            --indent;
        } else {
            int sign = 0;
            int newIndent = indent;
            if (value[0] == '+') {
                value.erase(0, 1);
                sign = 1;
            } else if (value[0] == '-') {
                value.erase(0, 1);
                sign = -1;
            }

            fromString(value, newIndent);
            if (sign != 0)
                newIndent = indent + sign * newIndent;
            NGREST_ASSERT(newIndent < 1024 && newIndent >= 0, "Invalid indentation: " + toString(newIndent)
                          + " while processing line: \n" + line);
            indent = newIndent;
        }
    }

    void process(std::istream& fsIn, std::ostream& out, const xml::Element& element)
    {
        std::string line;
        std::stringbuf data;

        while (!fsIn.eof() && fsIn.good()) {
            if (fsIn.peek() == '\n') {
                line = "\n";
            } else {
                fsIn.get(data, '\n');
                line = data.str();
                if (line.size() > 0 && *line.rbegin() == '\\') {
                    line.erase(line.size() - 1);
                } else {
                    if (fsIn.peek() == '\n')
                        line += "\n";
                }
                data.str("");
            }

            fsIn.ignore();
            fsIn.peek(); // for EOF

            if (line.substr(0, 3) == "###") {
                // comment
            } else if (line.substr(0, 7) == "##echo ") {
                std::string value = line.substr(7);
                replaceToValue(value, element);
                std::cout << value << std::endl;
            } else if (line.substr(0, 6) == "##var ") {
                std::string::size_type pos = line.find_first_of(" \t", 6);
                std::string variable;
                std::string value;

                if (pos == std::string::npos) {
                    variable = line.substr(6);
                    stringTrimRight(variable);
                } else {
                    variable = line.substr(6, pos - 6);
                    value = line.substr(pos + 1, line.size() - pos - 2);
                    replaceToValue(value, element);
                }

                NGREST_ASSERT(!variable.empty(), "invalid var declaration: " + line);

                variables.top()[variable] = value;
            } else if (line.substr(0, 7) == "##ifeq(") {
                processIfeq(fsIn, out, element, line);
            } else if (line.substr(0, 8) == "##ifneq(") {
                processIfeq(fsIn, out, element, line, true);
            } else if (line.substr(0, 9) == "##switch ") {
                processSwitch(fsIn, out, element, line);
            } else if (line.substr(0, 10) == "##foreach ") {
                processForEach(fsIn, out, element, line);
            } else if (line.substr(0, 10) == "##context ") {
                processContext(fsIn, out, element, line);
            } else if (line.substr(0, 11) == "##fileopen ") {
                std::string fileName = line.substr(11);
                replaceToValue(fileName, element);
                stringTrim(fileName);

                NGREST_ASSERT(!fileName.empty(), "##fileopen: Filename is empty");

                fileName = outDir + fileName;
                const std::string& failedFileName = fileName + ".failed";

                std::ofstream ofsFile(fileName.c_str());
                NGREST_ASSERT(ofsFile.good(), "can't open output file: " + fileName);

                std::cout << "Generating " << fileName << std::endl;
                indent = 0;
                try {
                    process(fsIn, ofsFile, element);
                } catch(...) {
                    ofsFile.close();
                    ::unlink(failedFileName.c_str());
                    ::rename(fileName.c_str(), failedFileName.c_str());
                    throw;
                }
                ofsFile.close();
                ::unlink(failedFileName.c_str());
            } else if (line.substr(0, 11) == "##fileclose") {
                return;
            } else if (line.substr(0, 8) == "##mkdir ") {
                std::string dirName = line.substr(7);
                replaceToValue(dirName, element);
                stringTrim(dirName);

                File(outDir + dirName).mkdirs();
            } else if (line.substr(0, 10) == "##include ") {
                line.erase(0, 10);
                stringTrim(line);
                processInclude(out, element, line);
            } else if (line.substr(0, 10) == "##warning ") {
                replaceToValue(line, element);
                stringTrimRight(line, "\n\r");
                std::cerr << "Warning: " << line.substr(10) << std::endl;
            } else if (line.substr(0, 8) == "##error ") {
                replaceToValue(line, element);
                NGREST_THROW_ASSERT(line.substr(8));
            } else if (line.substr(0, 9) == "##indent ") {
                processIndent(line);
            } else if (line.substr(0, 10) == "##pushvars") {
                variables.push(variables.top());
            } else if (line.substr(0, 9) == "##popvars") {
                variables.pop();
            } else if (line.substr(0, 10) == "##dumpvars") {
                const StringMap& rmVars = variables.top();
                line = "variables dump:";
                for (StringMap::const_iterator itVar = rmVars.begin(); itVar != rmVars.end(); ++itVar)
                    line += "\n" + itVar->first + "=\"" + itVar->second + "\"";
                out << line << "\n";
            } else if (!line.empty()) {
                std::string indentStr;
                NGREST_ASSERT(indent < 1024, "Invalid indentation: " + toString(indent));
                for (int i = 0; i < indent; ++i) {
                    indentStr += "    ";
                }
                if (isNeedIndent && line[0] != '\n') {
                    line = indentStr + line;
                    isNeedIndent = false;
                }

                std::string::size_type begin = 0;
                std::string::size_type end = 0;
                while ((end = line.find('\n', begin)) != std::string::npos) {
                    if (isNeedIndent && end > begin) { // line is not empty
                        line.insert(begin, indentStr);
                        end += indentStr.size();
                    }

                    begin = end + 1;

                    isNeedIndent = true;
                }

                replaceToValue(line, element);

                out << line;
            }
            ++this->line;
        }
    }

    void processFile(const std::string& in, const std::string& outDir, const std::string& outFile,
                     const xml::Element& elementInterface, bool updateOnly, bool& isNeedUpdate)
    {
        const std::string& out = outDir + outFile;
        if (updateOnly) {
            bool bIsStaticTemplate = elementInterface.getName() == "Project";
            int inTime = File(in).getTime();
            int outTime = File(out).getTime();

            if (bIsStaticTemplate) {
                if (!isNeedUpdate && outTime && outTime > inTime) {
                    std::cout << "Skipping " << out << std::endl;
                    return;
                }
            } else {
                if (outTime && outTime > inTime) {
                    const std::string& fileName = outDir + elementInterface.getChildElementByName("fileName").getTextValue();
                    int64_t interfaceTime = File(fileName).getTime();

                    if (outTime && outTime > interfaceTime) {
                        std::cout << "Skipping " << out << std::endl;
                        return;
                    }

                    isNeedUpdate = true;
                }
            }
        }

        {
            std::ifstream fsIn;
            std::ofstream fsOut;

            fsIn.open(in.c_str());
            NGREST_ASSERT(fsIn.good(), "can't open input file: " + in);

            fsOut.open(out.c_str());
            if (!fsOut.good()) {
                fsIn.close();
                NGREST_THROW_ASSERT("can't open output file: " + out);
            }

            std::cout << "Generating " << out << std::endl;

            line = 0;
            indent = 0;
            isNeedIndent = true;
            process(fsIn, fsOut, elementInterface);

            fsIn.close();
            fsOut.close();
        }
    }

    void setEnv(const StringMap& env)
    {
        variables.top() = env;
    }

private:
    std::list<std::string> templateFileList;
    std::list<std::string> constFileList;
    mutable std::stack<StringMap> variables;
    std::string inDir;
    std::string outDir;
    int line;
    int indent;
    bool isNeedIndent;
    bool hasConfig;
};


void CodeGen::start(const std::string& templateDir, const std::string& outDir,
                    const xml::Element& rootElement, bool updateOnly, const StringMap& env)
{
    TemplateParser templateParser;

    File(outDir).mkdirs();

    templateParser.init(templateDir + NGREST_PATH_SEPARATOR);
    templateParser.setEnv(env);
    templateParser.start(outDir, rootElement, updateOnly);
}
}
}
