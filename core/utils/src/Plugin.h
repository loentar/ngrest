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

template <typename PluginType>
class Plugin: public DynamicLibrary
{
public:
    void load(const std::string& path)
    {
        DynamicLibrary::load(path, true);

#ifdef WIN32
        typedef PluginType* (*ngrestGetPlugin)();
        PNgrestGetPluginAddress getPluginAddr = reinterpret_cast<ngrestGetPlugin>(
                    getSymbol(NGREST_PLUGIN_EXPORTED_SYMBOL_STR));
        NGREST_ASSERT(getPluginAddr, "Error while getting NgrestGetPluginAddress");
        pluginSymbol = reinterpret_cast<PluginType*>(getPluginAddr());
#else
        pluginSymbol = reinterpret_cast<PluginType*>(getSymbol(NGREST_PLUGIN_EXPORTED_SYMBOL_STR));
#endif

        NGREST_ASSERT(pluginSymbol, "Error while getting plugin object");
    }

    inline PluginType* getPluginSymbol()
    {
        return pluginSymbol;
    }

private:
    PluginType* pluginSymbol;
};

}

#endif // NGREST_UTILS_PLUGIN_H
