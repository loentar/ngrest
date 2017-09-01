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

#ifndef _CPPPARSER_H_
#define _CPPPARSER_H_

#include <ngrest/codegen/CodegenParser.h>

namespace ngrest {
namespace codegen {

/**
 * @brief C++ header parser class
 */
class CppParser: public CodegenParser
{
public:
    /**
     * @brief get plugin id
     * @return plugin id
     */
    virtual const std::string& getId();

    /**
     * @brief process project with given settings
     * @param parseSettings project parse settings
     * @param project resuling project
     */
    virtual void process(const ParseSettings& parseSettings, Project& project);

private:
    static const std::string id;
};

}
}

#endif // _CPPPARSER_H_
