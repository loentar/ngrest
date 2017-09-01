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

#ifndef NGREST_CODEGEN_H
#define NGREST_CODEGEN_H

#include <string>
#include <ngrest/codegen/Interface.h>

namespace ngrest {

namespace xml {
class Element;
}

namespace codegen {

/**
 * @brief Code generator
 */
class CodeGen
{
public:
    /**
     * @brief start code generation
     * @param templateDir - path to templates
     * @param outDir - output directory
     * @param rootElement - root element, describing project
     * @param updateOnly - true: update files if needed, false: always update files
     * @param env - environment
     */
    void start(const std::string& templateDir, const std::string& outDir,
               const xml::Element& rootElement, bool updateOnly, const StringMap& env);
};
}
}

#endif // NGREST_CODEGEN_H
