// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Duktape/duktape.h>

#include <Atomic/Core/Profiler.h>
#include <Atomic/Core/CoreEvents.h>

#include <Atomic/IO/File.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/PackageFile.h>

#include <Atomic/Resource/ResourceCache.h>

#include "JSRequire.h"
#include "JSPlugin.h"
#include "JSEvents.h"
#include "JSVM.h"
#include "JSAtomic.h"
#include "JSUI.h"
#include "JSMetrics.h"
#include "JSEventHelper.h"

namespace Atomic
{

JSVM* JSVM::instance_ = NULL;

JSVM::JSVM(Context* context) :
    Object(context),
    ctx_(0),
    gcTime_(0.0f)
{
    assert(!instance_);

    instance_ = this;

    metrics_ = new JSMetrics(context, this);

    context_->RegisterSubsystem(new JSEventDispatcher(context_));
    context_->SetGlobalEventListener(context_->GetSubsystem<JSEventDispatcher>());

}

JSVM::~JSVM()
{
    context_->SetGlobalEventListener(0);
    context_->RemoveSubsystem(JSEventDispatcher::GetTypeStatic());

    duk_destroy_heap(ctx_);
    instance_ = NULL;
}

void JSVM::InitJSContext()
{
    ctx_ = duk_create_heap_default();

    jsapi_init_atomic(this);

    // register whether we are in the editor
    duk_get_global_string(ctx_, "Atomic");
    duk_push_boolean(ctx_, context_->GetEditorContext() ? 1 : 0);
    duk_put_prop_string(ctx_, -2, "editor");
    duk_pop(ctx_);

    duk_push_global_stash(ctx_);
    duk_push_object(ctx_);
    duk_put_prop_index(ctx_, -2, JS_GLOBALSTASH_INDEX_COMPONENTS);
    duk_pop(ctx_);

    js_init_require(this);
    js_init_jsplugin(this);    

    ui_ = new JSUI(context_);

    // handle this elsewhere?
    SubscribeToEvents();

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
        duk_gc(ctx_, 0);
        duk_gc(ctx_, 0);

        gcTime_ = 0;

    }

    duk_get_global_string(ctx_, "__js_atomic_main_update");

    if (duk_is_function(ctx_, -1))
    {
        duk_push_number(ctx_, timeStep);
        duk_pcall(ctx_, 1);
        duk_pop(ctx_);
    }

    else
    {
        duk_pop(ctx_);
    }
}

bool JSVM::ExecuteFunction(const String& functionName)
{
    duk_get_global_string(ctx_, functionName.CString());
    if (duk_is_function(ctx_, -1))
    {
        bool ok = true;

        if (duk_pcall(ctx_, 0) != 0)
        {
            ok = false;
            if (duk_is_object(ctx_, -1))
            {
                SendJSErrorEvent();
            }
            else
            {
                assert(0);
            }
                    }

        duk_pop(ctx_);
        return ok;

    }
    else
    {
        duk_pop(ctx_);
    }

    return false;

}

void JSVM::SendJSErrorEvent(const String& filename)
{
    duk_context* ctx = GetJSContext();

    using namespace JSError;

    VariantMap eventData;

    if (duk_is_string(ctx, -1))
    {
        eventData[P_ERRORNAME] = "(Unknown Error Name)";
        eventData[P_ERRORFILENAME] = "(Unknown Filename)";
        eventData[P_ERRORLINENUMBER] =  -1;
        eventData[P_ERRORMESSAGE] = duk_to_string(ctx, -1);
        eventData[P_ERRORSTACK] = "";
        SendEvent(E_JSERROR, eventData);
        return;
    }

    assert(duk_is_object(ctx, -1));

    duk_get_prop_string(ctx, -1, "fileName");

    if (duk_is_string(ctx, -1))
    {
        eventData[P_ERRORFILENAME] = duk_to_string(ctx, -1);
    }
    else
    {
        eventData[P_ERRORFILENAME] = filename;
    }

    // Component script are wrapped within a closure, the line number
    // needs to be offset by this header
    duk_get_prop_string(ctx, -2, "lineNumber");
    int lineNumber = (int) (duk_to_number(ctx, -1));

    eventData[P_ERRORLINENUMBER] =  lineNumber;

    duk_get_prop_string(ctx, -3, "name");
    String name = duk_to_string(ctx, -1);
    eventData[P_ERRORNAME] = name;

    duk_get_prop_string(ctx, -4, "message");
    String message = duk_to_string(ctx, -1);
    eventData[P_ERRORMESSAGE] = message;

    // we're not getting good file/line from duktape on parser errors
    if (name == "SyntaxError")
    {
        lineNumber = -1;
        // parse line if we have it
        if (message.Contains("(line "))
        {
            if (!filename.Length())
                eventData[P_ERRORFILENAME] = lastModuleSearchFilename_;

            unsigned pos = message.Find("(line ");
            const char* parse = message.CString() + pos + 6;
            String number;
            while (*parse >= '0' && *parse<='9')
            {
                number += *parse;
                parse++;
            }

            lineNumber = ToInt(number);

        }

        eventData[P_ERRORLINENUMBER] =  lineNumber;

    }

    duk_get_prop_string(ctx, -5, "stack");
    String stack = duk_to_string(ctx, -1);
    eventData[P_ERRORSTACK] = stack;

    duk_pop_n(ctx, 5);

    LOGERRORF("JSErrorEvent: %s : Line %i\n Name: %s\n Message: %s\n Stack:%s",
              filename.CString(), lineNumber, name.CString(), message.CString(), stack.CString());

    SendEvent(E_JSERROR, eventData);

}

int JSVM::GetRealLineNumber(const String& fileName, const int lineNumber) {
    int realLineNumber = lineNumber;
    String path = fileName;
    if (!path.EndsWith(".js.map"))
        path += ".js.map";
    if (path.EndsWith(".js")) {
        return realLineNumber;
    }
    SharedPtr<File> mapFile(GetSubsystem<ResourceCache>()->GetFile(path));
    //if there's no source map file, maybe you use a pure js, so give an error, or maybe forgot to generate source-maps :(
    if (mapFile.Null()) 
    {
        return realLineNumber;
    }    
    String map;
    mapFile->ReadText(map);
    int top = duk_get_top(ctx_);
    duk_get_global_string(ctx_, "require");
    duk_push_string(ctx_, "AtomicEditor/Script/jsutils");
    if (duk_pcall(ctx_, 1))
    {
        printf("Error: %s\n", duk_safe_to_string(ctx_, -1));
        duk_set_top(ctx_, top);
        return false;
    }

    duk_get_prop_string(ctx_, -1, "getRealLineNumber");
    duk_push_string(ctx_, map.CString());
    duk_push_int(ctx_, lineNumber);
    bool ok = true;
    if (duk_pcall(ctx_, 2))
    {
        ok = false;
        printf("Error: %s\n", duk_safe_to_string(ctx_, -1));
    }
    else
    {
        realLineNumber = duk_to_int(ctx_, -1);
    }
    duk_set_top(ctx_, top);

    return realLineNumber;
}

bool JSVM::ExecuteScript(const String& scriptPath)
{
    String path = scriptPath;
    if (!path.StartsWith("Scripts/"))
        path = "Scripts/" + path;

    if (!path.EndsWith(".js"))
        path += ".js";

    SharedPtr<File> file (GetSubsystem<ResourceCache>()->GetFile(path));

    if (file.Null())
    {
        return false;
    }

    String source;

    file->ReadText(source);
    source.Append('\n');

    duk_push_string(ctx_, file->GetFullPath().CString());
    if (duk_eval_raw(ctx_, source.CString(), 0,
                     DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN) != 0)
    {
        if (duk_is_object(ctx_, -1))
            SendJSErrorEvent(path);

        duk_pop(ctx_);
        return false;
    }

    duk_pop(ctx_);

    return true;
}

bool JSVM::ExecuteFile(File *file)
{
    if (!file)
        return false;

    String source;

    file->ReadText(source);
    source.Append('\n');

    duk_push_string(ctx_, file->GetFullPath().CString());
    if (duk_eval_raw(ctx_, source.CString(), 0,
        DUK_COMPILE_EVAL | DUK_COMPILE_SAFE | DUK_COMPILE_NOSOURCE | DUK_COMPILE_STRLEN) != 0)
    {
        SendJSErrorEvent(file->GetFullPath());

        duk_pop(ctx_);
        return false;
    }

    duk_pop(ctx_);

    return true;
}

void JSVM::GC()
{
    // run twice to ensure finalizers are run
    duk_gc(ctx_, 0);
    duk_gc(ctx_, 0);
}

bool JSVM::ExecuteMain()
{
    if (!GetSubsystem<ResourceCache>()->Exists("Scripts/main.js"))
        return true;


    duk_get_global_string(ctx_, "require");
    duk_push_string(ctx_, "Scripts/main");

    if (duk_pcall(ctx_, 1) != 0)
    {
        SendJSErrorEvent();
        return false;
    }

    if (duk_is_object(ctx_, -1))
    {
        duk_get_prop_string(ctx_, -1, "update");

        if (duk_is_function(ctx_, -1))
        {
            // store function for main loop
            duk_put_global_string(ctx_, "__js_atomic_main_update");
        }
        else
        {
            duk_pop(ctx_);
        }

    }

    // pop main module
    duk_pop(ctx_);

    return true;

}

}
