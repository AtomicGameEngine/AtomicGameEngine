//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#include <assert.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

/*
// subsystem requires

#include <Atomic/Engine/Engine.h>
#include <Atomic/Graphics/Graphics.h>
#include <Atomic/Graphics/Renderer.h>
#include <Atomic/Input/Input.h>
#include <Atomic/Resource/ResourceCache.h>
*/

#include "JSVM.h"
#include "JSRequire.h"
#include "JSPlugin.h"

namespace Atomic
{

/*
    static bool js_require_subsystem(const String& name, JSVM* vm)
    {
        duk_context* ctx = vm->GetJSContext();

        String lowername = name.ToLower();

        if (lowername.ToLower() == "input")
        {
            js_push_class_object_instance(ctx, vm->GetSubsystem<Input>());
            return true;
        }

        if (lowername.ToLower() == "resourcecache")
        {
            js_push_class_object_instance(ctx, vm->GetSubsystem<ResourceCache>());
            return true;
        }

        if (lowername.ToLower() == "engine")
        {
            js_push_class_object_instance(ctx, vm->GetSubsystem<Engine>());
            return true;
        }

        if (lowername.ToLower() == "renderer")
        {
            js_push_class_object_instance(ctx, vm->GetSubsystem<Renderer>());
            return true;
        }

        if (lowername == "graphics")
        {
            js_push_class_object_instance(ctx, vm->GetSubsystem<Graphics>());
            return true;
        }

        if (lowername.ToLower() == "vm")
        {
            js_push_class_object_instance(ctx, vm);
            return true;
        }

        return false;
    }
*/

    // see http://duktape.org/guide.html#modules
    static int js_module_search(duk_context* ctx)
    {
        JSVM* vm = JSVM::GetJSVM(ctx);
        FileSystem* fs = vm->GetSubsystem<FileSystem>();
        ResourceCache* cache = vm->GetSubsystem<ResourceCache>();

        int top = duk_get_top(ctx);

        assert(top ==  4);

        String moduleID = duk_to_string(ctx, 0);

        if (top > 1)
        {
            // require function
            assert(duk_is_function(ctx, 1));
        }

        if (top > 2)
        {
            // exports
            assert(duk_is_object(ctx, 2));
        }

        if (top > 3)
        {
            // module (module.id == a resolved absolute identifier for the module being loaded)
            assert(duk_is_object(ctx, 3));
        }

        String pathName, fileName, extension;
        SplitPath(moduleID, pathName, fileName, extension);
        String path = moduleID;

        // It is nice to not have to specify the Atomic path, but verify that the module exists first since it could be user provided
        if (fileName.StartsWith("Atomic") && cache->Exists("AtomicModules/" + path + ".js"))
        {
            path = "AtomicModules/" + path + ".js";
        }
        else
        {
            path += ".js";

            if (!cache->Exists(path))
            {
                const Vector<String>& searchPaths = vm->GetModuleSearchPaths();
                for (unsigned i = 0; i < searchPaths.Size(); i++)
                {
                    String search = searchPaths[i] + path;
                    if (cache->Exists(search))
                    {
                        path = search;
                        break;
                    }
                }
            }
        }

        if (cache->Exists(path))
        {
            SharedPtr<File> jsfile(cache->GetFile(path, false));
            vm->SetLastModuleSearchFile(jsfile->GetFullPath());
            String source;
            jsfile->ReadText(source);
            source.Append('\n');
            duk_push_string(ctx, source.CString());
            return 1;
        }
        else
        {
            // we're not a JS file, so check if we're a native module
            const Vector<String>& resourceDirs = cache->GetResourceDirs();

            for (unsigned i = 0; i < resourceDirs.Size(); i++)
            {

             String pluginLibrary;

             // TODO: proper platform folder detection
#ifdef ATOMIC_PLATFORM_WINDOWS
              pluginLibrary = resourceDirs.At(i) + "Plugins/Windows/x64/" + moduleID + ".dll";
#elif ATOMIC_PLATFORM_OSX
             pluginLibrary = resourceDirs.At(i) + "Plugins/Mac/x64/lib" + moduleID + ".dylib";
#endif

               if (pluginLibrary.Length() && fs->FileExists(pluginLibrary))
                {
                    // let duktape know we loaded a native module
                    if (jsplugin_load(vm, pluginLibrary))
                    {
                        duk_push_undefined(ctx);
                        return 1;
                    }
                    else
                    {
                        duk_push_sprintf(ctx, "Failed loading native plugins: %s", pluginLibrary.CString());
                        duk_throw(ctx);
                    }
                }
            }

        }

        duk_push_sprintf(ctx, "Failed loading module: %s", path.CString());
        duk_throw(ctx);

    }

    void js_init_require(JSVM* vm)
    {
        duk_context* ctx = vm->GetJSContext();
        duk_get_global_string(ctx, "Duktape");
        duk_push_c_function(ctx, js_module_search, 4);
        duk_put_prop_string(ctx, -2, "modSearch");
        duk_pop(ctx);
    }

}
