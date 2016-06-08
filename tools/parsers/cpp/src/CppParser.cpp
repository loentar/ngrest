/*
 *  Copyright 2016 Utkin Dmitry <loentar@gmail.com>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*
 *  This file is part of ngrest: http://github.com/loentar/ngrest
 */

#include <limits.h>
#include <errno.h>
#include <string.h>
#include <list>
#include <iostream>
#include <fstream>
#include <sstream>
#include <ngrest/utils/Log.h>
#include <ngrest/utils/stringutils.h>
#include <ngrest/utils/PluginExport.h>
#include <ngrest/utils/Exception.h>
#include <ngrest/codegen/tools.h>
#include "CppParser.h"

namespace ngrest {
namespace codegen {

class CppHeaderParser
{
public:
    CppHeaderParser(const std::string& rootNs):
        rootNamespace(rootNs), currentNs(rootNs), line(1), project(nullptr)
    {
    }

    void skipWs()
    {
        char ch = 0;
        while (file.good() && !file.eof()) {
            ch = file.peek();
            if (ch == '\n')
                ++line;

            if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t') {
                if (ch == '/') { // comment
                    file.ignore();
                    if (file.peek() == '/') { // single line comment
                        file.ignore(INT_MAX, '\n');
                        ++line;
                        continue;
                    } else if (file.peek() == '*') { // multiline comment
                        file.ignore();
                        while (file.good() && !file.eof()) {
                            file.get(ch);
                            if (ch == '*') {
                                file.get(ch);
                                if (ch == '/')
                                    break;
                            }

                            if (ch == '\n')
                                ++line;
                        }
                    } else {
                        file.unget();
                        break;
                    }
                } else {
                    break;
                }
            }
            file.ignore();
        }
    }

    void skipWsOnly()
    {
        char ch = 0;
        while (file.good() && !file.eof()) {
            ch = file.peek();
            if (ch == '\n')
                ++line;

            if (ch != ' ' && ch != '\n' && ch != '\r' && ch != '\t')
                break;
            file.ignore();
        }
    }

    void skipWsInLine()
    {
        char ch = 0;
        while (file.good() && !file.eof()) {
            ch = file.peek();

            if (ch != ' ' && ch != '\t')
                break;
            file.ignore();
        }
    }

    void skipSingleLineComment()
    {
        char ch = '\0';
        while (file.good() && !file.eof()) {
            ch = file.peek();
            if (ch != ' ' && ch != '\t')
                break;
            file.ignore();
        }

        if (file.good() && !file.eof()) {
            if (file.peek() == '/') { // comment
                file.ignore();
                if (file.peek() == '/') { // single line comment
                    file.ignore(INT_MAX, '\n');
                    ++line;
                } else {
                    file.unget();
                }
            }
        }
    }

    void readStr(std::string& string, bool isSkipWS = true)
    {
        string.erase();
        if (isSkipWS)
            skipWs();
        file >> string;
    }

    void readBefore(std::string& out, const std::string& delim = " \r\n\t,();[]{}<>\\/*-+!@#$%^&*=")
    {
        char ch = 0;

        out.erase();

        while (file.good() && !file.eof()) {
            ch = file.peek();
            if (delim.find(ch) != std::string::npos)
                break;

            if (ch == '\n')
                ++line;

            out += ch;
            file.ignore();
        }
    }

    void readDescrComment(std::string& description)
    {
        char ch = '\0';
        while (file.good() && !file.eof()) {
            ch = file.peek();
            if (ch != ' ' && ch != '\t')
                break;
            file.ignore();
        }

        if (file.good() && !file.eof()) {
            if (file.peek() == '/') { // comment
                file.ignore();
                if (file.peek() == '/') { // single line comment
                    file.ignore();
                    if (file.peek() == '!') {
                        file.ignore();
                        if (file.peek() == '<') { // description
                            file.ignore();
                            readBefore(description, "\n\r");
                            stringTrim(description);
                        }
                    }
                    file.ignore(INT_MAX, '\n');
                    ++line;
                } else {
                    file.unget();
                }
            }
        }
    }

    bool readComment(std::string& comment)
    {
        if (file.peek() != '/')
            return false;

        comment.erase();

        file.ignore();
        if (file.peek() == '/') {
            file.ignore();
            readBefore(comment, "\n\r");
        } else if (file.peek() == '*') {
            char ch = '\0';
            std::string tmp;

            file.ignore();
            while (file.good() && !file.eof()) {
                readBefore(tmp, "*");

                comment += tmp;
                file.get(ch);
                if (ch == '*') {
                    if (file.peek() == '/') {
                        file.ignore();
                        break;
                    }
                    comment += ch;
                }

                if (ch == '\n')
                    ++line;
            }
        } else {
            file.unget();
        }

        return !comment.empty();
    }

    bool checkIgnoreMetacomment(const std::string& text)
    {
        bool result = false;
        if (text.substr(1, 11) == "ignoreBegin") {
            // start ignore block
            std::string line;
            do {
                readBefore(line, "\n");
                file.ignore();
                stringTrim(line);
                if (line.size() >= 12 && line[0] == '/' && line[1] == '/') {
                    line.erase(0, 2);
                    stringTrim(line);
                    if (line.substr(0, 10) == "*ignoreEnd") {
                        result = true;
                        break;
                    }
                }
            }
            while (file.good());
        }

        return result;
    }

    template<typename StructType>
    bool parseCompositeDataType(const std::list<StructType>& types, DataType& dataType)
    {
        for (const StructType& type : types) {
            if (type.name == dataType.name) {
                // namespace match
                if (type.ns == dataType.ns)
                    return true;

                std::string::size_type pos = currentNs.find_last_of("::");

                while (pos != std::string::npos) {
                    ++pos;
                    if ((currentNs.substr(0, pos) + dataType.ns) == type.ns) {
                        if (type.ns != dataType.ns) // correct namespace
                            dataType.ns = type.ns;

                        return true;
                    }

                    if (pos < 4)
                        break;

                    pos = currentNs.find_last_of("::", pos - 3);
                }
            }
        }

        return false;
    }

    void getDataType(const std::string& dataTypeName, DataType& dataType)
    {
        std::string::size_type pos = dataTypeName.find_last_of("::");
        if (pos != std::string::npos) {
            ++pos;
            dataType.name = dataTypeName.substr(pos);
            dataType.ns = dataTypeName.substr(0, pos);
        } else {
            dataType.name = dataTypeName;
            dataType.ns = ""; //"::";
        }

        if (dataTypeName == "ngrest::Node" ||
                (dataTypeName == "Node" && currentNs.substr(0, 10) == "::ngrest::")) {
            dataType.type = DataType::Type::DataObject;
        } else if (dataTypeName == "ngrest::MessageContext" || dataTypeName == "ngrest::Optional" ||
                   ((dataTypeName == "MessageContext" || dataTypeName == "Optional")
                    && currentNs.substr(0, 10) == "::ngrest::")) {
            dataType.type = DataType::Type::Generic; // supress unknown datatype warning
        } else if (dataTypeName == "bool" ||
                   dataTypeName == "char" ||
                   dataTypeName == "int" ||
                   dataTypeName == "float" ||
                   dataTypeName == "double" ||
                   dataTypeName == "void") {
            dataType.type = DataType::Type::Generic;
        } else if (dataTypeName == "std::string" || dataTypeName == "std::wstring") {
            dataType.type = DataType::Type::String;
        } else if (parseCompositeDataType(interface.typedefs, dataType)) {
            dataType.type = DataType::Type::Typedef;
        } else {
            // long type names detection. example: unsigned long int
            bool isGeneric = true;
            {
                bool isLong = false;
                bool isShort = false;
                bool isSigned = false;
                bool isUnsigned = false;
                bool isChar = false;
                bool isInt = false;
                bool isFloat = false;

                for (std::string::size_type begin = 0, end = 0;
                     end != std::string::npos; begin = end + 1) {

                    end = dataTypeName.find_first_of(" \t\n\r", begin);
                    std::string type = dataTypeName.substr(begin, end - begin);
                    stringTrim(type);

                    if (type == "long") {
                        CSP_ASSERT(!isShort, "short and long type", interface.fileName, line);
                        CSP_ASSERT(!isChar, "char and long type", interface.fileName, line);
                        CSP_ASSERT(!isFloat, "float and long type", interface.fileName, line);
                        isLong = true;
                    } else if (type == "short") {
                        CSP_ASSERT(!isLong, "long and short type", interface.fileName, line);
                        CSP_ASSERT(!isChar, "char and short type", interface.fileName, line);
                        CSP_ASSERT(!isFloat, "float and short type", interface.fileName, line);
                        isShort = true;
                    } else if (type == "signed") {
                        CSP_ASSERT(!isUnsigned, "signed and unsigned type", interface.fileName, line);
                        isSigned = true;
                    } else if (type == "unsigned") {
                        CSP_ASSERT(!isSigned, "signed and unsigned type", interface.fileName, line);
                        isUnsigned = true;
                    } else if (type == "char") {
                        CSP_ASSERT(!isShort, "short and char type", interface.fileName, line);
                        CSP_ASSERT(!isLong, "long and char type", interface.fileName, line);
                        CSP_ASSERT(!isFloat, "char and float type", interface.fileName, line);
                        isChar = true;
                    } else if (type == "double" || type == "float") {
                        CSP_ASSERT(!isShort, "short and float type", interface.fileName, line);
                        CSP_ASSERT(type == "double" || !isLong, "long and float type", interface.fileName, line);
                        CSP_ASSERT(!isSigned, "signed and float type", interface.fileName, line);
                        CSP_ASSERT(!isUnsigned, "unsigned and float type", interface.fileName, line);
                        CSP_ASSERT(!isChar, "char and float type", interface.fileName, line);
                        CSP_ASSERT(!isInt, "int and float type", interface.fileName, line);
                        isFloat = true;
                    } else if (type == "int") {
                        CSP_ASSERT(!isChar, "char and int type", interface.fileName, line);
                        CSP_ASSERT(!isFloat, "float and int type", interface.fileName, line);
                        isInt = true;
                    } else {
                        isGeneric = false;
                    }
                }

                dataType.name = dataTypeName;
            }
            dataType.type = isGeneric ? DataType::Type::Generic : DataType::Type::Unknown;
        }
    }

    void ignoreFunction()
    {
        char ch = '\0';
        int recursion = 0;

        while (file.good() && !file.eof()) {
            skipWs();
            file >> ch;

            if (ch == ';' && recursion == 0) {
                skipSingleLineComment();
                break;
            } else if (ch == '}') {
                --recursion;
                CSP_ASSERT(recursion >= 0, "mismatch {}", interface.fileName, line);

                if (recursion == 0) {
                    while (!file.eof() && file.peek() == ';')
                        file.ignore();
                    break;
                }
            } else if (ch == '{')  {
                ++recursion;
            } else if (ch == '#') { // skip {} in preprocessor
                while (file.good() && !file.eof() && ch != '\n')
                    file >> ch;
            } else if (ch == '"' || ch == '\'') { // skip {} in strings
                char quote = ch;
                while (file.good() && !file.eof()) {
                    char prevCh = ch;
                    file >> ch;
                    if (ch == quote && prevCh != '\\')
                        break;
                }

            }
        }
    }

    std::string::size_type parseTemplate(const std::string& templ, DataType& dataType,
                                         const Struct* parent)
    {
        std::string::size_type result = 0;
        std::string token;
        for (std::string::size_type begin = 0, end = 0;
             end != std::string::npos; begin = end + 1) {
            end = templ.find_first_of(",<>", begin);
            if (end == std::string::npos) {
                token = templ.substr(begin);
            } else {
                if (templ[end] == '<') {
                    int recurse = 1;

                    ++end;
                    for (; recurse > 0; ++end) {
                        end = templ.find_first_of("<>", end);
                        CSP_ASSERT(end != std::string::npos,
                                   " error while parsing template params: \"<" + templ + ">\" unmatched open/close",
                                   interface.fileName, line);

                        if (templ[end] == '<') {
                            ++recurse;
                        } else if (templ[end] == '>') {
                            --recurse;
                        }
                    }
                }

                token = templ.substr(begin, end - begin);
            }

            stringTrim(token);
            if (!token.empty()) {
                DataType templParam;
                parseDataType(token, templParam, parent);
                dataType.params.push_back(templParam);
            }

            if (templ[end] == '>') {
                result = end + 1;
                break;
            }
        }

        return result;
    }

    // datatype
    void parseDataType(const std::string& str, DataType& dataType, const Struct* parent = nullptr)
    {
        std::string tmp;

        bool isRef = false;

        dataType.isConst = false;
        dataType.isRef = false;
        dataType.type = DataType::Type::Unknown;
        dataType.usedName.erase();
        dataType.params.clear();
        dataType.ns.erase();

        std::string typeName;

        std::string::size_type size = str.size();

        for (std::string::size_type begin = 0, end = 0;
             end != std::string::npos && end < size; begin = end) {
            end = str.find_first_of(" \n\r\t&<*", begin);
            if (end != std::string::npos) {
                tmp = str.substr(begin, end  - begin);
                end = str.find_first_not_of(" \n\r\t", end); // skip whitespaces
            } else {
                tmp = str.substr(begin);
            }

            if (tmp == "static") {
                LogWarning() << "operations cannot be static. Line:" << line;
                CSP_ASSERT(typeName.empty(), "static after typename", interface.fileName, line);
            } else if (tmp == "struct") {
                dataType.prefix = tmp;
                tmp = str.substr(tmp.size());
                stringTrim(tmp);
                end = std::string::npos;
            }

            if (end != std::string::npos) {
                char ch = str[end];
                if (ch == '<') {
                    std::string::size_type pos = tmp.find_last_of("::");
                    if (pos != std::string::npos) {
                        ++pos;
                        dataType.name = tmp.substr(pos);
                        dataType.ns = tmp.substr(0, pos);
                    } else {
                        dataType.name = tmp;
                        dataType.ns.erase();
                    }

                    dataType.type = DataType::Type::Template;

                    ++end;

                    std::string::size_type templateSize = parseTemplate(str.substr(end), dataType, parent);
                    end += templateSize;
                    continue;
                } else if (ch == '&') {
                    CSP_ASSERT(typeName.empty(), "reference before typename: [" + str + "]",
                               interface.fileName, line);
                    isRef = true;
                    ++end;
                } else if (ch == '*') {
                    CSP_THROW("pointers are not supported in service declaration. in: ["
                              + str + "]", interface.fileName, line);
                }
            }

            if (tmp == "const") {
                CSP_ASSERT(typeName.empty(), "const after type declaration: ["
                           + str + "]", interface.fileName, line);
                dataType.isConst = true;
            } else if (!tmp.empty()) { // name of type
                CSP_ASSERT(!dataType.isRef, "type after reference: [" + str + "]",
                           interface.fileName, line);
                if (!typeName.empty())
                    typeName += ' ';

                typeName += tmp;
            }

            dataType.isRef = isRef;
        }

        if (dataType.type == DataType::Type::Unknown)
            getDataType(typeName, dataType);

        if (dataType.type == DataType::Type::Unknown) {
            const BaseType* baseType = getBaseType(typeName, interface, BaseType::Type::Any, parent);
            if (!baseType)
                baseType = getBaseType(currentNs + typeName, interface, BaseType::Type::Any, parent);

            if (baseType) {
                dataType.type = (baseType->type == BaseType::Type::Struct)
                        ? DataType::Type::Struct : DataType::Type::Enum;
                dataType.ns = baseType->ns;
                dataType.name = baseType->name;
                if ((baseType->type == BaseType::Type::Struct || BaseType::Type::Enum) &&
                        !baseType->ownerName.empty())
                    dataType.name = baseType->ownerName + "::" + dataType.name;

                dataType.usedName = typeName;
            } else {
                LogWarning() << "Can't find type declaration: " << str;
            }
        }
    }

    // parameter
    void parseParam(Param& param)
    {
        param.description.erase();

        // detect templates
        std::string paramAndType;
        int ltCount = 0;
        int gtCount = 0;
        for (;;) {
            std::string tmp;
            readBefore(tmp, ",)");
            if (tmp.empty())
                break;

            for (const std::string::value_type ch : tmp) {
                switch (ch) {
                case '<':
                    ++ltCount;
                    break;
                case '>':
                    ++gtCount;
                    break;
                }
            }

            paramAndType += tmp;

            if (ltCount == gtCount)
                break;

            paramAndType += file.get();
        }


        // read param type and name
        stringTrim(paramAndType);
        std::string::size_type pos = paramAndType.find_last_of(" \n\r\t");
        CSP_ASSERT(pos != std::string::npos, "Can't get param name: [" + paramAndType + "]",
                   interface.fileName, line);
        std::string dataType = paramAndType.substr(0, pos);
        stringTrim(dataType);

        parseDataType(dataType, param.dataType);
        param.name = paramAndType.substr(pos + 1);
    }

    // operation
    void parseOperation(const std::string& ret, Operation& operation)
    {
        Param param;
        char ch = 0;
        std::string tmp;

        operation.isConst = false;
        operation.isAsynch = false;

        skipWs();

        // read return type and operation name
        std::string operationAndType;
        readBefore(operationAndType, "(");
        operationAndType = ret + " " + operationAndType;
        stringTrim(operationAndType);
        std::string::size_type pos = operationAndType.find_last_of(" \n\r\t");
        CSP_ASSERT(pos != std::string::npos, "Can't get operation name: [" + operationAndType + "]",
                   interface.fileName, line);
        std::string dataType = operationAndType.substr(0, pos);
        stringTrim(dataType);

        parseDataType(dataType, operation.returnType.dataType);

        CSP_ASSERT(!operation.returnType.dataType.isRef, "return value cannot be reference",
                   interface.fileName, line);

        operation.name = operationAndType.substr(pos + 1);

        skipWs();
        file >> ch;
        CSP_ASSERT(ch == '(', "'(' expected after function name", interface.fileName, line);

        skipWs(); // arguments?
        ch = file.peek();
        if (ch == ')') {
            file.ignore();
        } else {
            while (file.good()) {
                CSP_ASSERT(!file.eof(), "unexpected EOF(after operation name)",
                           interface.fileName, line);

                parseParam(param); // reading param
                if (param.dataType.name == "Callback") {
                    if (!param.dataType.isRef)
                        LogWarning() << "Callback must defined as reference: \n"
                                     << "ngrest::Callback<ReturnType>& callback\n"
                                     << " in operation: " << operation.name
                                     << " in " << interface.fileName << ":" << line;

                    CSP_ASSERT(!param.dataType.params.empty(),
                               "Callback must define asynchronous return type: "
                               "ngrest::Callback<ReturnType>& callback",
                               interface.fileName, line);

                    CSP_ASSERT(operation.returnType.dataType.name == "void",
                               "Asynchronous operation must have void return type",
                               interface.fileName, line);

                    operation.isAsynch = true;
                } else if (param.dataType.isRef && !param.dataType.isConst &&
                           param.dataType.name != "MessageContext") {
                    LogWarning() << "Non-const reference to " << param.dataType.name
                                 << " in operation: " << operation.name
                                 << " in " << interface.fileName << ":" << line
                                 << " \n(return value cannot be passed over argument)";
                }

                skipWs();
                file >> ch; // more arguments?
                operation.params.push_back(param);

                if (ch == ')')
                    break;

                CSP_ASSERT(ch == ',', "error parsing param", interface.fileName, line);
            }
        }

        skipWs();
        ch = file.peek();
        if (ch == 'c') {
            readBefore(tmp, ";");
            if (tmp == "const")
                operation.isConst = true;

            skipWs();
            file >> ch;
        } else {
            ignoreFunction();
        }

        skipSingleLineComment();
    }

    // class
    void parseService(Service& service)
    {
        char ch = '\0';
        std::string tmp;

        skipWs();
        service.ns = currentNs;

        readStr(tmp);
        CSP_ASSERT(!file.eof(), "unexpected EOF(after clasname and '{')", interface.fileName, line);
        CSP_ASSERT(tmp == "{", "'{' after clasname expected ", interface.fileName, line);

        // parsing operations
        while (file.good() && !file.eof()) {
            Operation operation;
            skipWsOnly();
            while (readComment(tmp)) {
                stringTrim(tmp);
                if (tmp.size() != 0) {
                    if (tmp[0] == '*') {
                        // codegen metacomment
                        if (!checkIgnoreMetacomment(tmp)) {
                            std::string::size_type pos = tmp.find(':', 1);
                            if (pos != std::string::npos) {
                                // add an option
                                std::string name = tmp.substr(1, pos - 1);
                                std::string value = tmp.substr(pos + 1);
                                stringTrim(name);
                                stringTrim(value);
                                operation.options[name] = value;
                            }
                        }
                    }
                    else if (tmp[0] == '!') {
                        std::string descriptionTmp = tmp.substr(1);
                        stringTrim(descriptionTmp);
                        if (operation.description.size() != 0) {
                            if (operation.details.size() != 0)
                                operation.details += '\n';
                            operation.details += descriptionTmp;
                        } else {
                            operation.description = descriptionTmp;
                        }
                    }
                }
                skipWsOnly();
            }

            ch = file.peek();
            if (ch == '}') {
                file.ignore();
                break;
            }

            readStr(tmp);

            if (tmp == "public:") {
            } else if (tmp == "private:" || tmp == "protected:") {
                LogWarning() << "all operations of interface class must be only public!";
            } else if (tmp.substr(0, service.name.size()) == service.name) {
                // constructor-ignore it
                ignoreFunction();
            } else if (tmp.substr(0, service.name.size() + 1) == "~" + service.name) {
                // destructor
                ignoreFunction();
            } else if (tmp == "enum") {
                // enum -ignore
                LogWarning() << "Enum in service definition: ignored";
                ignoreFunction();
            } else {
                skipWs();
                parseOperation(tmp, operation);
                for (StringMap::const_iterator itOption = operation.options.begin();
                     itOption != operation.options.end(); ++itOption) {
                    const std::string& optionName = itOption->first;
                    if (optionName.substr(0, 6) == "param-") {
                        std::string::size_type pos = optionName.find_last_of('-');
                        if (pos == std::string::npos) {
                            LogError() << "Invalid param option: " << optionName;
                        } else {
                            const std::string& paramName = optionName.substr(6, pos - 6);

                            for (std::list<Param>::iterator itParam = operation.params.begin();
                                 itParam != operation.params.end(); ++itParam) {
                                if (itParam->name == paramName) {
                                    const std::string& name = optionName.substr(pos + 1);
                                    // add an option
                                    itParam->options[name] = itOption->second;
                                    break;
                                }
                            }
                        }
                    }
                }
                service.operations.push_back(operation);
            }
        }
    }

    void parseStruct(Struct& structure)
    {
        bool hasVirtualDtor = false;
        char ch = '\0';
        std::string tmp;
        bool isFunction = false;
        const std::string& ownerName = (!structure.ownerName.empty() ?
                                            (structure.ownerName + "::") : "") + structure.name;

        CSP_ASSERT(!file.eof(), "unexpected EOF(after struct name): " + structure.name,
                   interface.fileName, line);

        readStr(tmp);
        CSP_ASSERT(!file.eof(), "unexpected EOF(after structname): " + structure.name,
                   interface.fileName, line);

        if (tmp == ";") {
            file.unget();
            return;
        }

        CSP_ASSERT(structure.isForward, "Duplicating struct " + structure.name, interface.fileName, line);

        if (tmp == ":") {
            // inheritance
            readStr(tmp);
            CSP_ASSERT(! file.eof(), "unexpected EOF(after structname and inheritance sign): " + structure.name,
                       interface.fileName, line);

            if (tmp != "public") {
                LogWarning() << "non-public inheritance: " << structure.name << " => " << tmp;
            } else {
                readStr(tmp);
                CSP_ASSERT(!file.eof(), "unexpected EOF(while reading parent struct name): " + structure.name,
                           interface.fileName, line);
            }

            structure.parentName = tmp;
            readStr(tmp);
        }

        structure.isForward = false;

        CSP_ASSERT(tmp == "{", "'{' or ';' after structname expected: " + structure.name,
                   interface.fileName, line);

        while (file.good() && !file.eof()) {
            Field field;

            skipWsOnly();
            while (readComment(tmp)) {
                stringTrim(tmp);
                if (tmp.size() != 0) {
                    if (tmp[0] == '*') {
                        // codegen metacomment
                        if (!checkIgnoreMetacomment(tmp)) {
                            std::string::size_type pos = tmp.find(':', 1);
                            if (pos != std::string::npos) {
                                // add an option
                                std::string name = tmp.substr(1, pos - 1);
                                std::string value = tmp.substr(pos + 1);
                                stringTrim(name);
                                stringTrim(value);
                                field.options[name] = value;
                            }
                        }
                    } else if (tmp[0] == '!') {
                        // doxygen metacomment
                        field.description = tmp.substr(1);
                        stringTrim(field.description);
                    }
                }
                skipWsOnly();
            }

            isFunction = false;
            std::string token;
            readBefore(token);
            stringTrim(token);

            if (token == "enum") {
                std::string name;
                skipWs();
                readBefore(name, " \r\n\t;{}");

                if (name == "class") { // ignore "class" keyword after enum
                    skipWs();
                    readBefore(name, " \r\n\t;{}");
                }

                Enum& en = typeInList(structure.enums, name, currentNs, ownerName);

                parseEnum(en);
                if (!en.isForward) {
                    en.description = field.description;
                    en.options = field.options;
                }

                continue;
            }

            if (token == "struct") {
                std::string name;
                skipWs();
                readBefore(name, " \r\n\t;{}");

                bool isNestedStruct = false;

                if (!name.empty() && *name.rbegin() == ':') {
                    name.resize(name.size() - 1);
                    isNestedStruct = true;
                    file.unget();
                } else {
                    skipWs();
                    char ch = file.peek();
                    isNestedStruct = ch == ';' || ch == ':' || ch == '{';
                }

                if (isNestedStruct) {
                    // nested struct
                    Struct& st = typeInList(structure.structs, name, currentNs, ownerName);

                    st.options.insert(field.options.begin(), field.options.end());

                    parseStruct(st);
                    if (!st.isForward)
                        st.description = field.description;

                    continue;
                }

                // struct ::ns::Struct1 struct1;
                field.dataType.prefix = token;
                token += " " + name;
            }

            readBefore(tmp, ";}(");
            stringTrim(tmp);

            ch = file.peek();
            if (ch == '}') {
                CSP_ASSERT(tmp.empty(), "\";\" expected while parsing field. in struct: " + structure.name,
                           interface.fileName, line);
                file.ignore();
                break;
            }

            tmp = token + ' ' + tmp;
            stringTrim(tmp);

            if ((ch == '(') || // function
                    (token == structure.name) ||  // constructor
                    (token == ("~" + structure.name)))  // destructor
                isFunction = true;

            if (tmp == ("virtual ~" + structure.name))  // virtual destructor
                hasVirtualDtor = true;

            // struct operation
            if (!isFunction) {
                std::string::size_type nameBegin = tmp.find_last_of(" \n\r\t");
                CSP_ASSERT(nameBegin != std::string::npos, "Can't detect field name. in struct: "
                           + structure.name + " [" + tmp + "]", interface.fileName, line);

                field.name = tmp.substr(nameBegin + 1);

                tmp.erase(nameBegin);
                stringTrim(tmp);

                parseDataType(tmp, field.dataType, &structure);

                skipWs();
                file.get(ch);
                if (ch == ';') {
                    CSP_ASSERT(!field.dataType.isConst, "Struct operations must be non-const: " +
                               structure.name, interface.fileName, line);
                    CSP_ASSERT(!field.dataType.isRef, "Struct operations must be non-ref: " +
                               structure.name, interface.fileName, line);

                    readDescrComment(field.description);

                    structure.fields.push_back(field);
                } else {
                    isFunction = true;
                }
            }

            if (isFunction)
                ignoreFunction();
        }

        skipWsInLine();
        file >> ch;
        CSP_ASSERT(ch == ';', "missing ';' after struct definition", interface.fileName, line);

        skipSingleLineComment();

        StringMap::const_iterator itAbstract = structure.options.find("abstract");
        if (itAbstract != structure.options.end() &&
                (itAbstract->second == "true" || itAbstract->second == "1")) {
            CSP_ASSERT(hasVirtualDtor, "\nStructure [" + structure.ns + structure.name +
                       "] marked as abstract, but it does not have virtual destructor.\n",
                       interface.fileName, line);
        }
    }

    void parseEnum(Enum& en)
    {
        char ch = '\0';
        std::string tmp;

        CSP_ASSERT(!file.eof(), "unexpected EOF(after enum name): " + en.name,
                   interface.fileName, line);

        readStr(tmp);
        CSP_ASSERT(!file.eof(), "unexpected EOF(after enum name): " + en.name,
                   interface.fileName, line);

        if (tmp == ";") {
            file.unget();
            return;
        } else {
            CSP_ASSERT(en.isForward, "Duplicating enum " + en.name, interface.fileName, line);
        }

        en.isForward = false;

        CSP_ASSERT(tmp == "{", "'{' or ';' after enum name expected: " + en.name,
                   interface.fileName, line);

        while (file.good() && !file.eof()) {
            Enum::Member member;

            skipWsOnly();
            while (readComment(tmp)) {
                stringTrim(tmp);
                if (tmp.size() != 0) {
                    if (tmp[0] == '*') {
                        // codegen metacomment
                        if (!checkIgnoreMetacomment(tmp)) {
                            std::string::size_type pos = tmp.find(':', 1);
                            if (pos != std::string::npos) {
                                std::string name = tmp.substr(1, pos - 1);
                                stringTrim(name);
                                if (name == "value") {
                                    member.value = tmp.substr(pos + 1);
                                    stringTrim(member.value);
                                }
                            }
                        }
                    }
                }
                skipWsOnly();
            }

            readBefore(member.name);
            skipWs();

            ch = file.peek();
            if (ch == '=') {
                // read value
                file.ignore();
                skipWs();
                readBefore(member.value);
                skipWsOnly();
                if (!en.members.empty())
                    readDescrComment(en.members.back().description);
                skipWs();
                ch = file.peek();
            }

            if (!member.name.empty())
                en.members.push_back(member);

            if (ch == '}') {
                file.ignore();
                break;
            }

            CSP_ASSERT(ch == ',', "Expected \",\" or \"}\" after enum operation but \"" +
                       std::string(1, ch) + "\" found.", interface.fileName, line);

            file.ignore();

            readDescrComment(en.members.back().description);
        }

        skipWsInLine();
        file >> ch;
        CSP_ASSERT(ch == ';', "missing ';' after enum definition", interface.fileName, line);

        skipSingleLineComment();
    }

    void parseTypedef(Typedef& td)
    {
        std::string tmp;
        readBefore(tmp, ";");
        stringTrim(tmp);

        std::string::size_type nameBegin = tmp.find_last_of(" \n\r\t");
        CSP_ASSERT(nameBegin != std::string::npos, "Can't detect typedef name: [" + tmp + "]",
                   interface.fileName, line);

        td.name = tmp.substr(nameBegin + 1);

        tmp.erase(nameBegin);
        stringTrim(tmp);

        parseDataType(tmp, td.dataType);
        td.ns = currentNs;
    }

    void importEnums(const std::list<Enum>& src, std::list<Enum>& dst, const StringMap& options)
    {
        for (std::list<Enum>::const_iterator itEnum = src.begin();
             itEnum != src.end(); ++itEnum) {
            dst.push_back(*itEnum);
            Enum& en = dst.back();
            en.isExtern = true;
            // does not overwrite existing keys and values
            en.options.insert(options.begin(), options.end());
        }
    }

    void importStruct(const std::list<Struct>& rlsSrc, std::list<Struct>& rlsDst,
                      const StringMap& roptions)
    {
        for (std::list<Struct>::const_iterator itStruct = rlsSrc.begin();
             itStruct != rlsSrc.end(); ++itStruct) {
            rlsDst.push_back(Struct());
            Struct& st = rlsDst.back();
            st.name = itStruct->name;
            st.ns = itStruct->ns;
            st.parentName = itStruct->parentName;
            st.description = itStruct->description;
            st.details = itStruct->details;
            st.isExtern = true;
            st.ownerName = itStruct->ownerName;
            st.options = itStruct->options;
            // does not overwrite existing keys and values
            st.options.insert(roptions.begin(), roptions.end());
            importEnums(itStruct->enums, st.enums, roptions);
            importStruct(itStruct->structs, st.structs, roptions);
        }
    }

    void parsePreprocessorBlock()
    {
        std::string tmp;
        file.ignore();
        readStr(tmp, false);
        if (tmp == "include") {
            skipWs();
            char ch = file.peek();
            if (ch == '\"') {
                std::stringbuf sbTmp;
                file.ignore();
                file.get(sbTmp, ch);

                CppHeaderParser cppHeaderParser(rootNamespace);
                const Interface& interface = cppHeaderParser.parse(inDir, sbTmp.str(), *project);

                // import extern enums
                importEnums(interface.enums, this->interface.enums, interface.options);

                // import extern structs
                importStruct(interface.structs, this->interface.structs, interface.options);

                // use extern typedefs
                for (std::list<Typedef>::const_iterator itTypedef = interface.typedefs.begin();
                     itTypedef != interface.typedefs.end(); ++itTypedef) {
                    Typedef td = *itTypedef;
                    td.name = itTypedef->name;
                    td.ns = itTypedef->ns;
                    td.description = itTypedef->description;
                    td.isExtern = true;
                    this->interface.typedefs.push_back(td);
                }

                Include include;
                include.interfaceName = this->interface.name;
                include.ns = interface.ns;
                include.fileName = interface.fileName;
                include.filePath = interface.filePath;
                //          stInclude.targetNs = StringMapValue(interface.options, "targetNamespace");
                this->interface.includes.push_back(include);
            }
        }

        file.ignore(INT_MAX, '\n');
        ++line;
    }

    void parseHeaderBlock(Interface& interface)
    {
        char ch = 0;
        std::string description;
        std::string details;
        std::string tmp;
        StringMap options;
        StringList modules;

        skipWsOnly();
        while (readComment(tmp)) {
            stringTrim(tmp);
            if (tmp.size() != 0) {
                if (tmp[0] == '*') { // codegen metacomment
                    if (!checkIgnoreMetacomment(tmp)) {
                        std::string::size_type pos = tmp.find(':', 1);
                        if (pos != std::string::npos) {
                            // add an option
                            std::string name = tmp.substr(1, pos - 1);
                            std::string value = tmp.substr(pos + 1);
                            stringTrim(name);
                            stringTrim(value);
                            if (name == "engageModule") {
                                modules.push_back(value);
                            } else {
                                if (name.substr(0, 10) == "interface.") {
                                    interface.options[name.substr(10)] = value;
                                } else {
                                    options[name] = value;
                                }
                            }
                        }
                    }
                } else if (tmp[0] == '!') {
                    std::string descriptionTmp = tmp.substr(1);
                    stringTrimLeft(descriptionTmp);
                    if (description.size() != 0) {
                        if (details.size() != 0)
                            details += '\n';
                        details += descriptionTmp;
                    } else {
                        description = descriptionTmp;
                    }
                }
            }
            skipWsOnly();
        }

        ch = file.peek();
        if (ch == '#') { // preprocessor
            parsePreprocessorBlock();
            return;
        }

        if (ch == '}') // end of namespace
            return;

        readStr(tmp);

        if (tmp.size() == 0)
            return; // eof

        if (tmp == "class") {
            Service service;

            skipWs();

            // checking for service class
            readBefore(tmp, ":{;");
            stringTrimRight(tmp);

            std::string::size_type pos = tmp.find_last_of(" \t\n\r");
            if (pos == std::string::npos) {
                service.name = tmp;
            } else {
                service.name = tmp.substr(pos + 1);
                tmp.erase(pos);
                stringTrimRight(tmp);
                options["dllExport"] = tmp;
            }

            skipWs();
            char ch = file.peek();

            if (ch == ';') {
                file.ignore();
                return; // class forward
            }

            if (ch == '{') {
                ignoreFunction(); // ignore non-service class
                return;
            }

            if (ch == ':') // inheritance
                file.ignore();

            readStr(tmp);
            if (tmp != "public") {
                // not a service class
                readBefore(tmp, "{;");
                ignoreFunction(); // ignore non-service class
                return;
            }

            readStr(tmp);
            if (tmp != "Service" && tmp != "ngrest::Service") {
                // not a service class
                readBefore(tmp, "{;");
                ignoreFunction(); // ignore non-service class
                return;
            }

            LogDebug() << "Using [" << service.name << "] as service class";

            parseService(service);
            service.description = description;
            service.details = details;
            service.modules = modules;
            service.options = options;
            interface.services.push_back(service);

            skipWs();
            file >> ch;
            CSP_ASSERT(ch == ';', "missing ';' after class definition", interface.fileName, line);

            skipSingleLineComment();
            modules.clear();
            options.clear();
            description.erase();
            details.erase();
        } else if (tmp == "enum") {
            std::string name;
            skipWs();
            readBefore(name, " \r\n\t;{}");
            if (name == "class") { // ignore "class" keyword after enum
                skipWs();
                readBefore(name, " \r\n\t;{}");
            }

            Enum& en = typeInList(interface.enums, name, currentNs);

            parseEnum(en);
            if (!en.isForward) {
                en.description = description;
                en.details = details;
                en.options = options;
            }

            description.erase();
            details.erase();
        } else if (tmp == "struct") {
            std::string name;
            skipWs();
            readBefore(name, " \r\n\t:;{}");

            Struct& st = typeInList(interface.structs, name, currentNs);

            st.options.insert(options.begin(), options.end());

            parseStruct(st);
            if (!st.isForward) {
                st.description = description;
                st.details = details;
            }

            description.erase();
            details.erase();
        } else if (tmp == "typedef") {
            Typedef td;
            parseTypedef(td);

            skipWs();
            file >> ch;
            CSP_ASSERT(ch == ';', "missing ';' after typedef definition", interface.fileName, line);

            readDescrComment(td.description);

            td.options = options;
            interface.typedefs.push_back(td);

            description.erase();
            details.erase();
        } else if (tmp == "namespace") {
            std::string::size_type nsSize = currentNs.size();
            std::string ns;
            file >> ns;
            currentNs += ns + "::";

            parseBracketedBlock(interface);

            currentNs.erase(nsSize);

        } else if (tmp == ";") {
            skipSingleLineComment();
        } else if (ch == '#') { // preprocessor
            parsePreprocessorBlock();
        } else {
            CSP_THROW(("Unknown lexeme: \"" + tmp + "\""), interface.fileName, line);
        }
    }

    void parseBracketedBlock(Interface& interface)
    {
        char ch = 0;

        skipWs();
        file.get(ch);
        CSP_ASSERT(ch == '{', "parseBracketedBlock: \"{\" is not found!", interface.fileName, line);

        while (file.good() && !file.eof()) {
            skipWsOnly();
            ch = file.peek();

            if (ch == '#') { // preprocessor
                parsePreprocessorBlock();
            } else { // text
                if (ch == '}') {
                    file.ignore();
                    return;
                }

                if (ch == '{') {
                    parseBracketedBlock(interface);
                } else {
                    parseHeaderBlock(interface);
                }
            }
        }

        CSP_THROW("parseBracketedBlock: EOF found!", interface.fileName, line);
    }

    void parseHeader(Interface& interface)
    {
        char ch = 0;

        while (file.good() && !file.eof()) {
            skipWsOnly();
            ch = file.peek();

            if (ch == '{') {
                parseBracketedBlock(interface);
            } else if (ch == '}') {
                CSP_THROW("unexpected '}' found", interface.fileName, line);
            } else  {
                // text
                parseHeaderBlock(interface);
            }
        }
    }

    // Interface
    const Interface& parse(const std::string& inDir, const std::string& fileName, Project& project)
    {
        this->project = &project;
        this->inDir = inDir;

        std::string::size_type pos = fileName.find_last_of("/\\");
        const std::string& interfaceFileName = (pos != std::string::npos) ?
                    fileName.substr(pos + 1) : fileName;
        const std::string& interfaceFilePath = (pos != std::string::npos) ?
                    fileName.substr(0, pos + 1) : "";

        for (std::list<Interface>::const_iterator itInterface = project.interfaces.begin();
             itInterface != project.interfaces.end(); ++itInterface)
            if (itInterface->fileName == interfaceFileName &&
                    itInterface->filePath == interfaceFilePath)
                return *itInterface;

        interface.fileName = interfaceFileName;
        interface.filePath = interfaceFilePath;

        project.interfaces.push_back(interface);
        Interface& thisProjectInterface = project.interfaces.back();

        file.open((inDir + fileName).c_str());
        CSP_ASSERT(file.good(), std::string("can't open file: ") + fileName + ": "
                   + std::string(strerror(errno)), interface.fileName, line);
        try {
            interface.name = interface.fileName;
            const std::string::size_type size = interface.name.size();
            if (size > 2 && !interface.name.compare(size - 2, 2, ".h")) {
                interface.name.erase(size - 2);
            } else if (size > 4 && !interface.name.compare(size - 4, 4, ".hpp")) {
                interface.name.erase(size - 4);
            }

            parseHeader(interface);
            fixStuctParentNs();

            // detect interface main namespace
            if (interface.ns.empty()) {
                for (std::list<Service>::const_iterator itService = interface.services.begin();
                     itService != interface.services.end(); ++itService) {
                    if (!itService->ns.empty()) {
                        interface.ns = itService->ns;
                        break;
                    }
                }
            }

            if (interface.ns.empty()) {
                for (std::list<Struct>::const_iterator itStruct = interface.structs.begin();
                     itStruct != interface.structs.end(); ++itStruct) {
                    if (!itStruct->ns.empty() && !itStruct->isExtern) {
                        interface.ns = itStruct->ns;
                        break;
                    }
                }
            }

            if (interface.ns.empty()) {
                for (std::list<Typedef>::const_iterator itTypedef = interface.typedefs.begin();
                     itTypedef != interface.typedefs.end(); ++itTypedef) {
                    if (!itTypedef->ns.empty() && !itTypedef->isExtern) {
                        interface.ns = itTypedef->ns;
                        break;
                    }
                }
            }

            if (interface.ns.empty()) {
                for (std::list<Enum>::const_iterator itEnum = interface.enums.begin();
                     itEnum != interface.enums.end(); ++itEnum) {
                    if (!itEnum->ns.empty() && !itEnum->isExtern) {
                        interface.ns = itEnum->ns;
                        break;
                    }
                }
            }

            thisProjectInterface = interface;
            file.close();
        } catch (ParseException& parseException) {
            std::stringbuf data;
            file.get(data, '\n');
            file.ignore();
            file.get(data, '\n');
            std::string before = data.str();
            stringTrim(before);

            if (!before.empty())
                parseException.getMessage() += ": before\n-----------------\n"
                        + before + "\n-----------------\n";

            throw;
        }

        return interface;
    }

    void fixStuctParentNs()
    {
        // correct structs parent namespaces
        for (std::list<Struct>::iterator itStruct = interface.structs.begin();
             itStruct != interface.structs.end(); ++itStruct) {
            std::string& parentNs = itStruct->parentName;
            // skip structs without parent
            if (parentNs.empty())
                continue;

            const Struct* parent = getStruct(parentNs, interface);
            if (parent) {
                itStruct->parentName = parent->name;
                itStruct->parentNs = parent->ns;
            }
        }
    }

    std::string inDir;
    std::string rootNamespace;
    std::string currentNs;
    std::ifstream file;
    int line;
    Interface interface;
    Project* project;
};


const std::string& CppParser::getId()
{
    return id;
}

void CppParser::process(const ParseSettings& parseSettings, Project& project)
{
    unsigned servicesCount = 0;

    std::string rootNs = "::";
    StringMap::const_iterator itRootNs = parseSettings.env.find("rootns");
    if (itRootNs != parseSettings.env.end() && !itRootNs->second.empty()) {
        rootNs = "::" + itRootNs->second + "::";
        stringReplace(rootNs, ".", "::", true);
    }

    for (StringList::const_iterator itFile = parseSettings.files.begin();
         itFile != parseSettings.files.end(); ++itFile) {
        CppHeaderParser cppHeaderParser(rootNs);
        const Interface& interface = cppHeaderParser.parse(parseSettings.inDir, *itFile, project);
        servicesCount += interface.services.size();
    }

    StringMap::const_iterator itNs = parseSettings.env.find("projectns");
    if (itNs != parseSettings.env.end()) {
        project.ns = "::" + itNs->second + "::";
        stringReplace(project.ns, ".", "::", true);
    } else {
        // autodetect: take first defined namespace
        for (std::list<Interface>::const_iterator itInterface = project.interfaces.begin();
             itInterface != project.interfaces.end(); ++itInterface) {
            for (std::list<Service>::const_iterator itService = itInterface->services.begin();
                 itService != itInterface->services.end(); ++itService) {
                if (!itService->ns.empty()) {
                    project.ns = itService->ns;
                    break;
                }
            }

            if (!project.ns.empty())
                break;
        }
    }

    if (!servicesCount && !parseSettings.noServiceWarn && !project.interfaces.empty()) {
        LogWarning() << "No ngrest service interfaces found. "
                        "ngrest services must be inherited from ngrest::Service.\n"
                        "Example:\n----\n#include <ngrest/common/Service.h>"
                        "\n\n  class Calc: public ngrest::Service\n"
                        "  {\n  public:\n    "
                        "virtual int Add(int a, int b) = 0;\n  };\n----\n\n";
    }
}

const std::string CppParser::id = "cpp";

}
}

NGREST_DECLARE_PLUGIN(ngrest::codegen::CppParser)
