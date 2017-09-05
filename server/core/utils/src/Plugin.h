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

#ifndef NGREST_UTILS_PLUGIN_H
#define NGREST_UTILS_PLUGIN_H

#include "Exception.h"
#include "PluginExport.h"
#include "DynamicLibrary.h"

namespace ngrest {

/**
 * @brief plugin. exposes library through entry point
 */
template <typename PluginType>
class Plugin: public DynamicLibrary
{
public:
    /**
     * @brief load shared library by given path
     * @param path path to shared library
     */
    void load(const std::string& path)
    {
        DynamicLibrary::load(path, true);

#ifdef WIN32
        typedef PluginType* (*ngrestGetPlugin)();
        ngrestGetPlugin getPluginAddr = reinterpret_cast<ngrestGetPlugin>(getSymbol("ngrestGetPlugin"));
        NGREST_ASSERT(getPluginAddr, "Error getting address of ngrestGetPlugin");
        pluginSymbol = reinterpret_cast<PluginType*>(getPluginAddr());
#else
        pluginSymbol = reinterpret_cast<PluginType*>(getSymbol(NGREST_PLUGIN_EXPORTED_SYMBOL_STR));
#endif

        NGREST_ASSERT(pluginSymbol, "Error while getting plugin object");
    }

    /**
     * @brief get entry point exposed by library
     * @return plugin symbol
     */
    inline PluginType* getPluginSymbol()
    {
        return pluginSymbol;
    }

private:
    PluginType* pluginSymbol;
};

}

#endif // NGREST_UTILS_PLUGIN_H
