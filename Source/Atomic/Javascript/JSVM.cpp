
#include "Precompiled.h"

#include <Duktape/duktape.h>

#include "../Core/Profiler.h"
#include "../Core/CoreEvents.h"

#include "../IO/File.h"
#include "../IO/Log.h"
#include "../IO/FileSystem.h"
#include "../IO/PackageFile.h"

#include "../Resource/ResourceCache.h"

#include "../Javascript/JSEvents.h"
#include "../Javascript/JSFile.h"
#include "../Javascript/JSVM.h"
#include "../Javascript/JSAtomic.h"

namespace Atomic
{

JSVM* JSVM::instance_ = NULL;

JSVM::JSVM(Context* context) :
    Object(context),
    jsContext_(0),
    gcTime_(0.0f)
{
    assert(!instance_);

    instance_ = this;

    jsContext_ = duk_create_heap_default();

    // create root Atomic Object
    duk_push_global_object(jsContext_);
    duk_push_object(jsContext_);
    duk_put_prop_string(jsContext_, -2, "Atomic");
    duk_pop(jsContext_);

    duk_push_global_stash(jsContext_);
    duk_push_object(jsContext_);
    duk_put_prop_index(jsContext_, -2, JS_GLOBALSTASH_INDEX_COMPONENTS);
    duk_pop(jsContext_);

    duk_get_global_string(jsContext_, "Duktape");
    duk_push_c_function(jsContext_, js_module_search, 1);
    duk_put_prop_string(jsContext_, -2, "modSearch");
    duk_pop(jsContext_);

    jsapi_init_atomic(this);

    InitScripts();

    // handle this elsewhere?
    SubscribeToEvents();
}

JSVM::~JSVM()
{
    duk_destroy_heap(jsContext_);
    instance_ = NULL;
}

void JSVM::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, HANDLER(JSVM, HandleUpdate));
}

void JSVM::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    PROFILE(JSVM_HandleUpdate);

    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    gcTime_ += timeStep;
    if (gcTime_ > 5.0f)
    {
        PROFILE(JSVM_GC);

        // run twice to call finalizers
        // see duktape docs
        // also ensure #define DUK_OPT_NO_VOLUNTARY_GC
        // is enabled in duktape.h
        duk_gc(jsContext_, 0);
        duk_gc(jsContext_, 0);

        gcTime_ = 0;

    }

    duk_get_global_string(jsContext_, "__js_atomicgame_update");

    if (duk_is_function(jsContext_, -1))
    {
        duk_push_number(jsContext_, timeStep);
        duk_pcall(jsContext_, 1);
        duk_pop(jsContext_);
    }

    else
    {
        duk_pop(jsContext_);
    }
}

bool JSVM::ExecuteFunction(const String& functionName)
{
    duk_get_global_string(jsContext_, functionName.CString());
    if (duk_is_function(jsContext_, -1))
    {
        bool ok = true;

        if (duk_pcall(jsContext_, 0) != 0)
        {
            ok = false;
            if (duk_is_object(jsContext_, -1))
            {
                SendJSErrorEvent();
            }
            else
            {
                assert(0);
            }
                    }

        duk_pop(jsContext_);
        return ok;

    }
    else
    {
        duk_pop(jsContext_);
    }

    return false;

}

void JSVM::GenerateComponent(const String &cname, const String &jsfilename, const String& csource)
{
    String source = "(function() {\n function __component_function(self) {\n";

    source += csource;

    source += "self.node.components = self.node.components || {};\n";

    source.AppendWithFormat("self.node.components[\"%s\"] = self.node.components[\"%s\"] || [];\n",
                            cname.CString(), cname.CString());


    source += "if (typeof start != \"undefined\") self.start = start; " \
              "if (typeof update != \"undefined\") self.update = update; "\
              "if (typeof fixedUpdate != \"undefined\") self.fixedUpdate = fixedUpdate; " \
              "if (typeof postUpdate != \"undefined\") self.postUpdate = postUpdate;\n";

    String scriptName = cname;
    scriptName[0] = tolower(scriptName[0]);

    source.AppendWithFormat("self.node.%s = self.node.%s || self;\n",
                            scriptName.CString(), scriptName.CString());

    source.AppendWithFormat("self.node.components[\"%s\"].push(self);\n",
                            cname.CString());

    source += "}\n return __component_function;\n});";

    duk_push_string(jsContext_, jsfilename.CString());

    if (duk_eval_raw(jsContext_, source.CString(), source.Length(),
                     DUK_COMPILE_EVAL | DUK_COMPILE_NOSOURCE | DUK_COMPILE_SAFE) != 0)
    {
        if (duk_is_object(jsContext_, -1))
        {
            SendJSErrorEvent();
            duk_pop(jsContext_);
        }
        else
        {
            assert(0);
        }
    }
    else if (duk_pcall(jsContext_, 0) != 0)
    {
        if (duk_is_object(jsContext_, -1))
        {
            SendJSErrorEvent();
            duk_pop(jsContext_);
        }
        else
        {
            assert(0);
        }
    }
    else
    {
        if (!duk_is_function(jsContext_, -1))
        {
            const char* error = duk_to_string(jsContext_, -1);
            assert(false);
        }

        duk_put_prop_string(jsContext_, -2, cname.CString());
    }
}

void JSVM::InitPackageScripts()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    duk_push_global_stash(jsContext_);
    duk_get_prop_index(jsContext_, -1, JS_GLOBALSTASH_INDEX_COMPONENTS);

    const Vector<SharedPtr<PackageFile> >& packageFiles = cache->GetPackageFiles();

    for (unsigned i = 0; i < packageFiles.Size(); i++)
    {
        SharedPtr<PackageFile> package = packageFiles[i];
        const Vector<String>& files =  package->GetCaseEntryNames();

        for (unsigned j = 0; j < files.Size(); j++)
        {
            String name = files[j];
            if (!name.StartsWith("Components/"))
                continue;

            String cname = GetFileName(name);
            String jsname = name;

            JSFile* jsfile = cache->GetResource<JSFile>(name);
            String csource = jsfile->GetSource();

            GenerateComponent(cname, jsname, csource);

        }
    }

    // pop stash and component object
    duk_pop_2(jsContext_);

}

void JSVM::InitScripts()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();

    if (cache->GetPackageFiles().Size())
    {
        InitPackageScripts();
        return;
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    const Vector<String>& dirs = cache->GetResourceDirs();

    duk_push_global_stash(jsContext_);
    duk_get_prop_index(jsContext_, -1, JS_GLOBALSTASH_INDEX_COMPONENTS);

    for (unsigned i = 0; i < dirs.Size(); i++)
    {
        Vector<String> files;

        fileSystem->ScanDir(files ,dirs[i]+"/Components", "*.js", SCAN_FILES, true );

        for (unsigned j = 0; j < files.Size(); j++)
        {
            String cname = GetFileName(files[j]);
            String jsname = dirs[i]+"Components/" + files[j];

            JSFile* jsfile = cache->GetResource<JSFile>("Components/" + files[j]);

            String csource = jsfile->GetSource();

            GenerateComponent(cname, jsname, csource);

        }

    }

    // pop stash and component object
    duk_pop_2(jsContext_);

}

bool JSVM::ExecuteFile(JSFile* jsfile)
{
    PROFILE(ExecuteFile);

    bool ok = true;

    if (duk_peval_string(jsContext_, jsfile->GetSource()) != 0)
    {
        ok = false;
        printf("Error: %s\n", duk_safe_to_string(jsContext_, -1));
    }

    // ignore result
    duk_pop(jsContext_);

    return ok;
}

int JSVM::js_module_search(duk_context* ctx)
{
    JSVM* vm = GetJSVM(ctx);

    ResourceCache* cache = vm->GetContext()->GetSubsystem<ResourceCache>();

    String path = duk_to_string(ctx, 0);

    // first check if this is an AtomicModule
    // attempting to avoid module search paths
    // so have AtomicEditor, Atomic, and the projects "Modules"
    String pathName, fileName, extension;
    SplitPath(path, pathName, fileName, extension);

    if (fileName.StartsWith("AtomicEditor"))
    {
        path =  "AtomicEditor/Modules/" + path + ".js";
    }
    else if (fileName.StartsWith("Atomic"))
    {
        path =  "AtomicModules/" + path + ".js";
    }
    else
    {
        path = vm->moduleSearchPath_ + "/" + path + ".js";
    }

    JSFile* jsfile = cache->GetResource<JSFile>(path);
    if (!jsfile)
    {
        duk_push_null(ctx);
    }
    else
    {
        duk_push_string(ctx, jsfile->GetSource());
    }

    return 1;
}

/*
name	standard	Name of error, e.g. TypeError, inherited
message	standard	Optional message of error, own property, empty message inherited if absent
fileName	Rhino	Filename related to error source, inherited accessor
lineNumber	Rhino	Linenumber related to error source, inherited accessor
stack	V8	Traceback as a multi-line human redable string, inherited accessor


EVENT(E_JSERROR, JSError)
{
    PARAM(P_ERRORNAME, ErrorName); // string
    PARAM(P_ERRORMESSAGE, ErrorMessage); // string
    PARAM(P_ERRORFILENAME, ErrorFileName); // string
    PARAM(P_ERRORLINENUMBER, ErrorLineNumber); // int
    PARAM(P_ERRORSTACK, ErrorStack); // string
}
*/

void JSVM::SendJSErrorEvent()
{
    duk_context* ctx = GetJSContext();
    assert(duk_is_object(ctx, -1));

    using namespace JSError;

    VariantMap eventData;

    duk_get_prop_string(ctx, -1, "fileName");
    eventData[P_ERRORFILENAME] = duk_to_string(ctx, -1);

    // Component script are wrapped within a closure, the line number
    // needs to be offset by this header
    duk_get_prop_string(ctx, -2, "lineNumber");
    int lineNumber = (int) (duk_to_number(ctx, -1));

    eventData[P_ERRORLINENUMBER] =  lineNumber;

    duk_get_prop_string(ctx, -3, "name");
    eventData[P_ERRORNAME] = duk_to_string(ctx, -1);

    duk_get_prop_string(ctx, -4, "message");
    eventData[P_ERRORMESSAGE] = duk_to_string(ctx, -1);

    duk_get_prop_string(ctx, -5, "stack");
    eventData[P_ERRORSTACK] = duk_to_string(ctx, -1);

    duk_pop_n(ctx, 5);

    SendEvent(E_JSERROR, eventData);

}

bool JSVM::ExecuteMain()
{
    SharedPtr<JSFile> jsFile;
    jsFile = GetSubsystem<ResourceCache>()->GetResource<JSFile>("Script/Main.js");
    if (jsFile.Null())
        return false;

    int top = duk_get_top(jsContext_);

    // wrap in a closure
    String source = "(function() {\n function __main_function(self) {\n";

    source += "require(\"AtomicGame\");\n";

    source += jsFile->GetSource();

    source += "\nAtomic.game.init(Start, Update);\n";

    source += "}\n return __main_function;\n});";

    duk_push_string(jsContext_, "Main.js");

    if (duk_eval_raw(jsContext_, source.CString(), source.Length(),
                     DUK_COMPILE_EVAL | DUK_COMPILE_NOSOURCE | DUK_COMPILE_SAFE) != 0)
    {
        if (duk_is_object(jsContext_, -1))
        {
            SendJSErrorEvent();
            duk_pop(jsContext_);
            return false;
        }
        else
        {
            assert(0);
        }
    }
    else if (duk_pcall(jsContext_, 0) != 0)
    {
        if (duk_is_object(jsContext_, -1))
        {
            SendJSErrorEvent();
            duk_pop(jsContext_);
            return false;
        }
        else
        {
            assert(0);
        }
    }

    if (!duk_is_function(jsContext_, -1))
    {

        duk_pop(jsContext_);
        return false;
    }

    if (duk_pcall(jsContext_, 0) != 0)
    {
        if (duk_is_object(jsContext_, -1))
        {
            SendJSErrorEvent();
            duk_pop(jsContext_);
            return false;
        }
        else
        {
            assert(0);
        }
    }

    duk_pop(jsContext_);

    ExecuteFunction("__js_atomicgame_start");

    assert(top == duk_get_top(jsContext_));

    return true;

}


/*
void JSVM::DumpJavascriptObjects()
{
    HashMap<String, unsigned> counts;

    List<Object*>::Iterator itr = jsObjects_.Begin();
    while (itr != jsObjects_.End())
    {
        const String& tname = (*itr)->GetTypeName();
        if (!counts.Contains(tname))
        {
            counts[tname] = 0;
        }
        else
        {
            counts[tname]++;
        }
        itr++;
    }

    HashMap<String, unsigned>::Iterator oitr = counts.Begin();
    while (oitr != counts.End())
    {
        // currently outputting in destructor so log might already be gone
        //LOGINFOF("%s: %u", oitr->first_.CString(), oitr->second_);
        printf("%s: %u\n", oitr->first_.CString(), oitr->second_);

        oitr++;
    }

}
*/

}
