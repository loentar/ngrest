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

#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <list>
#include <ngrest/utils/Log.h>
#include <ngrest/utils/File.h>
#include <ngrest/utils/DynamicLibrary.h>
#include <ngrest/utils/Plugin.h>
#include <ngrest/utils/File.h>
#include <ngrest/utils/Runtime.h>
#include <ngrest/xml/Document.h>
#include <ngrest/xml/Element.h>
#include <ngrest/xml/XmlWriter.h>
#include <ngrest/codegen/Interface.h>
#include <ngrest/codegen/CodegenParser.h>
#include "CodeGen.h"
#include "XmlGen.h"
#if !defined VERSION_FULL && defined _MSC_VER
#include "version.h"
#endif

void help()
{
    std::cerr << "Code generator for ngrest\n"
                 "ngrestcg [source files][-t<template>][-p<plugin_name>][-i<inputdir>][-o<outputdir>][-c<chagedir>]"
                 "[-u][-e][-n<prj_name>][-x][-v]\n"
                 "  -t<template>    - Generate source with template name. Example: -tserviceimpl\n"
                 "  -p<plugin_name> - Use parser <plugin_name> to read source file(s). (default is cpp). Example: -pwsdl\n"
                 "  -i<inputdir>    - Set input dir\n"
                 "  -o<outputdir>   - Set output dir\n"
                 "  -c<changedir>   - Set input and output dirs\n"
                 "  -u              - Update only (generate only missing files, update existing files if needed)\n"
                 "  -e              - Don't warn if Interface file(s) does not contain a service\n"
                 "  -d              - Define environment variables: -dvar1=value1,var2=2,var3\n"
                 "  -l[t|p]         - Display parsers(p) and/or templates(t) lists\n"
                 "  -n<proj_name>   - Set project name and output file base for internal representation\n"
                 "  -x              - Write xml description\n"
                 "  -v              - Display current version and exit\n\n";
}

int main(int argc, const char* argv[])
{
    if (argc < 2) {
        help();
        return 1;
    }

    const std::string& codegenDir = ::ngrest::Runtime::getApplicationRootPath()
            + NGREST_PATH_SEPARATOR "share"
            NGREST_PATH_SEPARATOR "ngrest"
            NGREST_PATH_SEPARATOR "codegen";

    ngrest::codegen::ParseSettings parseSettings;
    ngrest::codegen::Project project;

    std::string templ;
    std::string pluginName = "cpp";
    bool generateXml = false;
    bool updateOnly = false;
    ngrest::codegen::StringMap env;

    const std::string& templatesDir = codegenDir + NGREST_PATH_SEPARATOR "templates" NGREST_PATH_SEPARATOR;
    const std::string& pluginsDir = codegenDir + "/parsers/";
    const std::string& pluginPrefix = "ngrestcgparser-";

    project.name = "project1";

    for (int i = 1; i < argc; ++i) {
        if (argv[i][0] == '-') {
            switch(argv[i][1]) {
            case 'h':
                help();
                return 0;

            case 'i':
                parseSettings.inDir = &argv[i][2];
                if (parseSettings.inDir.empty()) {
                    if (++i >= argc) {
                        std::cerr << "missing argument" << std::endl;
                        help();
                        return 1;
                    }
                    parseSettings.inDir = argv[i];
                }
                break;

            case 'o':
                parseSettings.outDir = &argv[i][2];
                if (parseSettings.outDir.empty()) {
                    if (++i >= argc) {
                        std::cerr << "missing argument" << std::endl;
                        help();
                        return 1;
                    }
                    parseSettings.outDir = argv[i];
                }
                break;

            case 'c':
                parseSettings.inDir = &argv[i][2];
                parseSettings.outDir = &argv[i][2];
                if (parseSettings.outDir.empty()) {
                    if (++i >= argc) {
                        std::cerr << "missing argument" << std::endl;
                        help();
                        return 1;
                    }
                    parseSettings.inDir = argv[i];
                    parseSettings.outDir = argv[i];
                }
                break;

            case 't':
                templ = &argv[i][2];
                if (templ.empty()) {
                    if (++i >= argc) {
                        std::cerr << "missing argument" << std::endl;
                        help();
                        return 1;
                    }
                    templ = argv[i];
                }
                break;

            case 'n':
                project.name = &argv[i][2];
                if (project.name.empty()) {
                    if (++i >= argc) {
                        std::cerr << "missing argument" << std::endl;
                        help();
                        return 1;
                    }
                    project.name = argv[i];
                }
                break;

            case 'x':
                generateXml = true;
                break;

            case 'u':
                updateOnly = true;
                break;

            case 'e':
                parseSettings.noServiceWarn = true;
                break;

            case 'd': {
                std::string vars = &argv[i][2];
                std::string var;
                std::string name;
                std::string value;

                if (vars.empty()) {
                    if (++i >= argc) {
                        std::cerr << "missing argument" << std::endl;
                        help();
                        return 1;
                    }
                    vars = argv[i];
                }


                std::string::size_type begin = 0;
                std::string::size_type end = 0;
                std::string::size_type pos = 0;
                while (end != std::string::npos) {
                    end = vars.find(',', begin);
                    var = vars.substr(begin, end - begin);

                    if (var.size() != 0) {
                        pos = var.find('=');
                        if (pos == std::string::npos) {
                            name = var;
                            value = "true";
                        } else {
                            name = var.substr(0, pos);
                            value = var.substr(pos + 1);
                        }

                        env[name] = value;
                    }

                    begin = end + 1;
                }
                break;
            }

            case 'p':
                pluginName = &argv[i][2];
                break;

            case 'l': {
                const std::string what = &argv[i][2];

                if (what.empty() || what.find('t') != std::string::npos) {
                    ngrest::codegen::StringList lsTemplates;

                    ngrest::File(templatesDir).list(lsTemplates, "*", ngrest::File::AttributeDirectory);
                    lsTemplates.sort();

                    std::cout << "Templates:\n";
                    for (ngrest::codegen::StringList::const_iterator itTemplate = lsTemplates.begin();
                         itTemplate != lsTemplates.end(); ++itTemplate) {
                        if (*itTemplate != "common") {
                            std::cout << "  " << *itTemplate << std::endl;
                        }
                    }
                }

                if (what.empty() || what.find('p') != std::string::npos) {
                    const std::string::size_type pluginPrefixSize = pluginPrefix.size();
                    const std::string::size_type pluginExtSize = strlen(NGREST_LIBRARY_EXT);
                    ngrest::codegen::StringList lsPlugins;
                    ngrest::File(pluginsDir).list(lsPlugins, pluginPrefix + "*" NGREST_LIBRARY_EXT,
                                                  ngrest::File::AttributeAnyFile);

                    lsPlugins.sort();

                    std::cout << "\nParsers:\n";
                    for (ngrest::codegen::StringList::const_iterator itPlugin = lsPlugins.begin();
                         itPlugin != lsPlugins.end(); ++itPlugin) {
                        std::cout << "  "
                                  << itPlugin->substr(pluginPrefixSize,
                                                      itPlugin->size() - pluginPrefixSize - pluginExtSize)
                                  << std::endl;
                    }
                }

                return 0;
            }

            case 'v':
                std::cout << "ngrestcg version " VERSION_FULL
                             "\n\nCopyright © 2016 Utkin Dmitry\n\n"
                             "Licensed under the Apache License, Version 2.0: "
                             "http://www.apache.org/licenses/LICENSE-2.0\n\n"
                             "For more information, please visit: https://github.com/loentar/ngrest"
                          << std::endl;
                return 0;

            default:
                std::cerr << "unrecognized option: " << argv[i] << std::endl << std::endl;
                help();
                return 1;
            }
        } else {
            if (argv[i][0] != '\0')
                parseSettings.files.push_back(argv[i]);
        }
    }

    if (!parseSettings.inDir.empty() &&
            parseSettings.inDir.substr(parseSettings.inDir.size() - 1) != NGREST_PATH_SEPARATOR)
        parseSettings.inDir += NGREST_PATH_SEPARATOR;

    if (!parseSettings.outDir.empty() &&
            parseSettings.outDir.substr(parseSettings.outDir.size() - 1) != NGREST_PATH_SEPARATOR)
        parseSettings.outDir += NGREST_PATH_SEPARATOR;


    try {
        // load plugin
        ngrest::Plugin<ngrest::codegen::CodegenParser> plugin;

        const std::string& fileName = pluginsDir + pluginPrefix + pluginName + NGREST_LIBRARY_EXT;

        plugin.load(fileName);
        ngrest::codegen::CodegenParser* codegenParser = plugin.getPluginSymbol();

        if (!codegenParser)
            CSP_THROW("Can't get plugin symbol " + fileName, "", 0);

        parseSettings.env = env;

        // Source files parsing
        codegenParser->process(parseSettings, project);

        // Generation
        ngrest::xml::Document doc;
        doc.getRootElement() << project;

        if (generateXml) {
            ::ngrest::File(parseSettings.outDir).mkdirs();
            const std::string& irFileName = parseSettings.outDir + project.name + ".xml";
            std::cout << "Generating " << irFileName << std::endl;
            doc.getDeclaration().setEncoding("UTF-8");

            ngrest::xml::XmlFileWriter xmlWriter(irFileName);
            xmlWriter.writeDocument(doc);
        }

        if (!templ.empty()) {
            ngrest::LogDebug() << "template: " << templ;

            NGREST_ASSERT(ngrest::File(templatesDir + templ).isDirectory(), "template [" + templ + "] not found");

            ngrest::codegen::CodeGen generator;
            generator.start(templatesDir + templ, parseSettings.outDir, doc.getRootElement(), updateOnly, env);
        }

        return 0;
    } catch (const ngrest::codegen::ParseException& exception) {
        ngrest::LogError() << exception;
    }
    NGREST_CATCH_ALL;

    return 1;
}
