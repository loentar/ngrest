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

#include <ngrest/utils/tostring.h>
#include "Interface.h"
#include "tools.h"
#include <set>

namespace ngrest {
namespace codegen {

std::string::size_type strIntersect(const std::string& string1, const std::string& string2)
{
    if (!string1.empty() && !string2.empty()) {
        std::string::size_type posA = string1.size() - 1;
        std::string::size_type posB = string2.size() - 1;
        std::string::size_type posA1;
        std::string::size_type posB1;
        const char* str1 = string1.c_str();
        const char* str2 = string2.c_str();

        for (; posB; --posB) {
            if (str1[posA] == str2[posB]) {
                posA1 = posA - 1;
                posB1 = posB - 1;
                for(; posA1 && posB1 && str1[posA1] == str2[posB1]; --posA1, --posB1);
                if (!posB1)
                    return posB + 1;
            }
        }
    }
    return std::string::npos;
}

const BaseType* getBaseType(const std::string& nsName, const Interface& interface,
                            const int baseType /*= BaseType::Type::Any*/,
                            const Struct* parent /*= nullptr*/)
{
    const Struct* curr = parent;
    const BaseType* result = nullptr;

    const std::string::size_type nsNameSize = nsName.size();

    if ((baseType & BaseType::Type::Typedef) != 0) {
        std::string findNsName = nsName;
        if (nsName.substr(0, 2) != "::")
            findNsName = "::" + nsName;

        for (std::list<Typedef>::const_reverse_iterator itTypedef = interface.typedefs.rbegin();
             itTypedef != interface.typedefs.rend(); ++itTypedef)
            if ((itTypedef->ns + itTypedef->name) == findNsName)
                return &*itTypedef;
    }

    // look substructs
    for (;;) {
        // to avod getting extern type with the same name as in current interface
        // we must search backwards, because extern types goes first
        const std::list<Struct>& structureList = !curr ? interface.structs : curr->structs;
        for (std::list<Struct>::const_reverse_iterator itStruct = structureList.rbegin();
             itStruct != structureList.rend(); ++itStruct) {
//          if (!itStruct->isForward) // skip forward declarations
            {
                std::string currNsName = itStruct->ns;
                if (!itStruct->ownerName.empty())
                    currNsName += itStruct->ownerName + "::";
                currNsName += itStruct->name;

                std::string::size_type currNsNameSize = currNsName.size();

                int sizeDiff = currNsNameSize - nsNameSize;

                //  full struct name with namespace
                // find in sub enums
                if (((baseType & BaseType::Type::Struct) != 0) && !sizeDiff && currNsName == nsName) {
                    result = &*itStruct;
                    break; // return
                } else {
                    // empty/partial namespace
                    if (sizeDiff >= 2) { // size of "::"
                        if (((baseType & BaseType::Type::Struct) != 0) &&
                                currNsName.substr(sizeDiff - 2, 2) == "::" &&
                                currNsName.substr(sizeDiff) == nsName) {
                            result = &*itStruct;
                            break; // return
                        }
                    }

                    // includes substruct name
                    // find intersection
                    // some::namespace::Struct X namespace::Struct::SubStruct = namespace::Struct
                    std::string::size_type pos = strIntersect(currNsName, nsName);
                    if (pos != std::string::npos
                            && (pos == currNsNameSize ||
                                (currNsNameSize >= (pos + 2) && currNsName.substr(currNsNameSize - pos - 2, 2) == "::"))
                            && (pos == nsName.size() || nsName.substr(pos, 2) == "::")) {
                        // go through child structs
                        pos += 2;
                        const Struct* tmpStruct = &*itStruct;
                        std::string::size_type begin = pos;
                        std::string::size_type end = 0;
                        do {
                            if (begin >= nsName.size()) {
                                tmpStruct = nullptr;
                                break;
                            }
                            end = nsName.find("::", begin);
                            const std::string& subName =
                                    end != std::string::npos ?
                                        nsName.substr(begin, end - begin) :
                                        nsName.substr(begin);
                            bool isFound = false;

                            // find in sub enums
                            if ((baseType & BaseType::Type::Enum) != 0) {
                                for (std::list<Enum>::const_reverse_iterator itSubEnum
                                     = tmpStruct->enums.rbegin();
                                     itSubEnum != tmpStruct->enums.rend(); ++itSubEnum) {
                                    if (itSubEnum->name == subName) {
                                        return &*itSubEnum;
                                    }
                                }
                            }

                            // go into sub struct
                            for (std::list<Struct>::const_reverse_iterator itSubStruct =
                                 tmpStruct->structs.rbegin();
                                 itSubStruct != tmpStruct->structs.rend(); ++itSubStruct) {
                                if (itSubStruct->name == subName) {
                                    tmpStruct = &*itSubStruct;
                                    isFound = true;
                                    break;
                                }
                            }

                            if (!isFound) {
                                tmpStruct = nullptr;
                                break;
                            }
                            begin = end + 2;
                        } while (end != std::string::npos);

                        if (tmpStruct) {
                            result = tmpStruct;
                            break;
                        }
                    }
                }
            }
        }

        // find in sub enums
        if (!result && ((baseType & BaseType::Type::Enum) != 0)) {
            const std::list<Enum>& renums = !curr ? interface.enums : curr->enums;
            for (std::list<Enum>::const_reverse_iterator itEnum = renums.rbegin();
                 itEnum != renums.rend(); ++itEnum) {
                std::string currNsName = itEnum->ns;
                if (!itEnum->ownerName.empty())
                    currNsName += itEnum->ownerName + "::";
                currNsName += itEnum->name;
                std::string::size_type currNsNameSize = currNsName.size();

                std::string::size_type pos = strIntersect(currNsName, nsName);
                if (pos != std::string::npos
                        && (pos == currNsNameSize ||
                            (currNsNameSize >= (pos + 2) && currNsName.substr(currNsNameSize - pos - 2, 2) == "::"))
                        && (pos == nsName.size() || nsName.substr(pos, 2) == "::")) {
                    result = &*itEnum;
                    break;
                }
            }
        }

        if (result || !curr)
            break;

        // find in parent owner struct
        curr = curr->ownerName.empty() ? nullptr : getStruct(curr->ns + curr->ownerName, interface);
    }

    return result;
}

const Struct* getStruct(const std::string& nsName, const Interface& interface,
                        const Struct* parent /*= nullptr*/)
{
    return static_cast<const Struct*>(getBaseType(nsName, interface, BaseType::Type::Struct, parent));
}


void optimizeCppNs(std::string& optimizeNs, const std::string& currentNs)
{
    // optimizeNs: ::samples::ticket::Ticket
    // currentNs: ::samples::sharedtypes::
    // result: ticket::

    std::string::size_type pos = 0;
    for (; pos < optimizeNs.size() &&
         pos < currentNs.size() &&
         optimizeNs[pos] == currentNs[pos]; ++pos);

    if (pos >= 2 && optimizeNs.substr(pos - 2, 2) == "::") {
        optimizeNs.erase(0, pos);
    }
}

void MangleCharInString(std::string::size_type& pos, std::string& result)
{
    static std::map<char, std::string> mapChars;
    if (mapChars.empty()) {
        mapChars['!'] = "_exclam_";
        mapChars['"'] = "_dquote_";
        mapChars['#'] = "_number_";
        mapChars['$'] = "_dollar_";
        mapChars['%'] = "_percent_";
        mapChars['&'] = "_amp_";
        mapChars['\''] = "_quote_";
        mapChars['('] = "_lpar_";
        mapChars[')'] = "_rpar_";
        mapChars['*'] = "_asterisk_";
        mapChars['+'] = "_plus_";
        mapChars[','] = "_comma_";
        mapChars['-'] = "_minus_";
        mapChars['.'] = "_period_";
        mapChars['/'] = "_slash_";
        mapChars[':'] = "_colon_";
        mapChars[';'] = "_semicolon_";
        mapChars['<'] = "_lt_";
        mapChars['='] = "_equal_";
        mapChars['>'] = "_gt_";
        mapChars['?'] = "_question_";
        mapChars['@'] = "_at_";
        mapChars['['] = "_lsbracket_";
        mapChars['\\'] = "_backslash_";
        mapChars[']'] = "_rsbracket_";
        mapChars['^'] = "_caret_";
        mapChars['`'] = "_grave_";
        mapChars['{'] = "_lcbrace_";
        mapChars['|'] = "_vbar_";
        mapChars['}'] = "_rcbrace_";
        mapChars['~'] = "_tilde_";
    }

    std::map<char, std::string>::const_iterator itSym = mapChars.find(result[pos]);
    if (itSym != mapChars.end()) {
        result.replace(pos, 1, itSym->second);
        pos += itSym->second.size();
    } else {
        const std::string& hex = '_' + toHexString(static_cast<unsignedByte>(result[pos]));
        result.replace(pos, 1, hex);
        pos += hex.size();
    }
}

bool fixFileName(std::string& fileName)
{
    static const char* szInvalidChars = "\\/:*?\"<>|";
    bool bChanged = false;

    std::string::size_type pos = 0;
    while ((pos = fileName.find_first_of(szInvalidChars, pos)) != std::string::npos) {
        fileName[pos] = '_';
        bChanged = true;
    }

    return bChanged;
}

bool fixId(std::string& id, bool ignoreBool /*= false*/)
{
    static const char* szIdChars = "_0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::set<std::string> setCppReservedWords;
    bool isChanged = false;

    if (id.empty())
        return false;

    std::string::size_type pos = 0;
    while ((pos = id.find_first_not_of(szIdChars, pos)) != std::string::npos) {
        MangleCharInString(pos, id);
        isChanged = true;
    }

    const char first = id[0];
    if (first >= '0' && first <= '9') {
        id.insert(0, "_", 1);
        return true;
    }

    if (setCppReservedWords.empty()) {
        const unsigned cppReservedWordsCount = 79;
        const char* cppReservedWords[cppReservedWordsCount] = {
            "and", "and_eq", "asm", "auto", "bitand", "bitor", "bool", "_Bool", "break", "case", "catch",
            "char", "class", "compl", "_Complex", "const", "const_cast", "continue", "default", "delete",
            "do", "double", "dynamic_cast", "else", "enum", "explicit", "export", "extern", "false", "float",
            "for", "friend", "goto", "if", "_Imaginary", "inline", "int", "long", "mutable", "namespace",
            "new", "not", "not_eq", "operator", "or", "or_eq", "private", "protected", "public", "register",
            "reinterpret_cast", "restrict", "return", "short", "signed", "sizeof", "static", "static_cast",
            "struct", "switch", "template", "this", "throw", "true", "try", "typedef", "typeid", "typename",
            "union", "unsigned", "using", "virtual", "void", "volatile", "wchar_t", "while", "xor", "xor_eq",
            "typeid"
        };
        for (unsigned index = 0; index < cppReservedWordsCount; ++index)
            setCppReservedWords.insert(cppReservedWords[index]);
    }

    if (setCppReservedWords.count(id)) {
        if (!ignoreBool || (id != "true" && id != "false")) {
            id += '_';
            isChanged = true;
        }
    }

    return isChanged;
}

const std::string& stringMapValue(const StringMap& map, const std::string& name,
                                  const std::string& defaultValue)
{
    StringMap::const_iterator itValue = map.find(name);
    return (itValue != map.end()) ? itValue->second : defaultValue;
}

const std::string& stringMapValue(const StringMap& map, const std::string& name)
{
    const static std::string defaultValue;
    StringMap::const_iterator itValue = map.find(name);
    return (itValue != map.end()) ? itValue->second : defaultValue;
}

}
}
