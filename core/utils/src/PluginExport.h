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

#ifndef NGREST_UTILS_PLUGINEXPORT_H
#define NGREST_UTILS_PLUGINEXPORT_H

#define NGREST_PLUGIN_EXPORTED_SYMBOL ngrestPlugin
#define NGREST_PLUGIN_EXPORTED_SYMBOL_STR "ngrestPlugin"

#ifdef WIN32
#define NGREST_DECLARE_PLUGIN(Clasname) \
    Clasname NGREST_PLUGIN_EXPORTED_SYMBOL; \
    extern "C" NGREST_DLL_EXPORT void* ngrestGetPlugin() \
{ return &NGREST_PLUGIN_EXPORTED_SYMBOL; }
#else
#define NGREST_DECLARE_PLUGIN(Clasname) \
    Clasname NGREST_PLUGIN_EXPORTED_SYMBOL;
#endif

#endif // NGREST_UTILS_PLUGINEXPORT_H
