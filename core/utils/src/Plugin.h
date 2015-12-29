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
