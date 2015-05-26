
#include <assert.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/ResourceCache.h>

#include "JSVM.h"
#include "JSRequire.h"
#include "JSPlugin.h"

namespace Atomic
{
    
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

        // Do we really want this?  It is nice to not have to specify the Atomic path
        if (fileName.StartsWith("Atomic"))
        {
            path = "AtomicModules/" + path + ".js";
        }
        else
        {
            path += ".js";

            if (!cache->Exists(path) && cache->Exists("Modules/" + path))
                path = "Modules/" + path;
 
        }

        if (cache->Exists(path))
        {
            SharedPtr<File> jsfile(cache->GetFile(path, false));
            vm->SetLastModuleSearchFile(jsfile->GetFullPath());
            String source;
            jsfile->ReadText(source);
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
