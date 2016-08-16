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
Vector<JSVM::JSAPIPackageRegistration*> JSVM::packageRegistrations_;

JSVM::JSVM(Context* context) :
    Object(context),
    ctx_(0),
    gcTime_(0.0f),
    stashCount_(0),
    totalStashCount_(0),
    totalUnstashCount_(0)
{
    assert(!instance_);

    instance_ = this;

    metrics_ = new JSMetrics(context, this);

    SharedPtr<JSEventDispatcher> dispatcher(new JSEventDispatcher(context_));
    context_->RegisterSubsystem(dispatcher);
    context_->AddGlobalEventListener(dispatcher);
    RefCounted::AddRefCountChangedFunction(OnRefCountChanged);

}

JSVM::~JSVM()
{
    context_->RemoveGlobalEventListener(context_->GetSubsystem<JSEventDispatcher>());
    context_->RemoveSubsystem(JSEventDispatcher::GetTypeStatic());

    duk_destroy_heap(ctx_);

    RefCounted::RemoveRefCountChangedFunction(OnRefCountChanged);

    // assert(stashCount_ == 0);

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

    js_init_require(this);
    js_init_jsplugin(this);

    ui_ = new JSUI(context_);

    InitializePackages();

    // handle this elsewhere?
    SubscribeToEvents();

}

void JSVM::InitializePackages()
{   
    for (unsigned i = 0; i < packageRegistrations_.Size(); i++)
    {
        JSAPIPackageRegistration* pkgReg = packageRegistrations_.At(i);

        if (pkgReg->registrationFunction)
        {
            pkgReg->registrationFunction(this);
        }
        else
        {
            pkgReg->registrationSettingsFunction(this, pkgReg->settings);
        }

        delete pkgReg;
    }

    packageRegistrations_.Clear();
    
}

void JSVM::RegisterPackage(JSVMPackageRegistrationFunction regFunction)
{
    packageRegistrations_.Push(new JSAPIPackageRegistration(regFunction));
}

void JSVM::RegisterPackage(JSVMPackageRegistrationSettingsFunction regFunction, const VariantMap& settings)
{
    packageRegistrations_.Push(new JSAPIPackageRegistration(regFunction, settings));
}

void JSVM::SubscribeToEvents()
{
    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(JSVM, HandleUpdate));
}

void JSVM::OnRefCountChanged(RefCounted* refCounted, int refCount)
{
    assert(instance_);
    assert(refCounted->JSGetHeapPtr());

    if (refCount == 1)
    {
        // only script reference is left, so unstash
        instance_->Unstash(refCounted);
    }
    else if (refCount == 2)
    {
        // We are going from solely having a script reference to having another reference
        instance_->Stash(refCounted);
    }

}

void JSVM::Stash(RefCounted* refCounted)
{
    assert(refCounted);
    assert(refCounted->JSGetHeapPtr());

    totalStashCount_++;
    stashCount_++;

    duk_push_global_stash(ctx_);
    duk_get_prop_index(ctx_, -1, JS_GLOBALSTASH_INDEX_REFCOUNTED_REGISTRY);
    // can't use instance as key, as this coerces to [Object] for
    // string property, pointer will be string representation of
    // address, so, unique key

/*
    duk_push_pointer(ctx_, refCounted);
    duk_get_prop(ctx_, -2);
    assert(duk_is_undefined(ctx_, -1));
    duk_pop(ctx_);
*/

    duk_push_pointer(ctx_, refCounted);
    duk_push_heapptr(ctx_, refCounted->JSGetHeapPtr());
    
    duk_put_prop(ctx_, -3);
    duk_pop_2(ctx_);

}
void JSVM::Unstash(RefCounted* refCounted)
{
    assert(refCounted);
    assert(refCounted->JSGetHeapPtr());

    assert(stashCount_ > 0);

    stashCount_--;
    totalUnstashCount_++;

    duk_push_global_stash(ctx_);
    duk_get_prop_index(ctx_, -1, JS_GLOBALSTASH_INDEX_REFCOUNTED_REGISTRY);
    // can't use instance as key, as this coerces to [Object] for
    // string property, pointer will be string representation of
    // address, so, unique key

/*
    duk_push_pointer(ctx_, refCounted);
    duk_get_prop(ctx_, -2);
    assert(!duk_is_undefined(ctx_, -1));
    duk_pop(ctx_);
*/

    duk_push_pointer(ctx_, refCounted);
    duk_del_prop(ctx_, -2);   
    duk_pop_2(ctx_);
}

// Returns if the given object is stashed
bool JSVM::GetStashed(RefCounted* refcounted) const
{
    duk_push_global_stash(ctx_);
    duk_get_prop_index(ctx_, -1, JS_GLOBALSTASH_INDEX_REFCOUNTED_REGISTRY);

    duk_push_pointer(ctx_, refcounted);
    duk_get_prop(ctx_, -2);

    bool result = !duk_is_undefined(ctx_, -1);

    duk_pop_3(ctx_);
    return result;
}


void JSVM::HandleUpdate(StringHash eventType, VariantMap& eventData)
{

    ATOMIC_PROFILE(JSVM_HandleUpdate);

    using namespace Update;

    // Take the frame time step, which is stored as a float
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    gcTime_ += timeStep;
    if (gcTime_ > 5.0f)
    {
        ATOMIC_PROFILE(JSVM_GC);

        // run twice to call finalizers
        // see duktape docs
        // also ensure #define DUK_OPT_NO_VOLUNTARY_GC
        // is enabled in duktape.h
        duk_gc(ctx_, 0);
        duk_gc(ctx_, 0);

        gcTime_ = 0;

        // DumpJavascriptObjects();

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

    ATOMIC_LOGERRORF("JSErrorEvent: %s : Line %i\n Name: %s\n Message: %s\n Stack:%s",
              filename.CString(), lineNumber, name.CString(), message.CString(), stack.CString());

    SendEvent(E_JSERROR, eventData);

}

int JSVM::GetRealLineNumber(const String& fileName, const int lineNumber) {

    int realLineNumber = lineNumber;

    String mapPath = fileName;

    if (!mapPath.EndsWith(".js.map"))
        mapPath += ".js.map";

    if (mapPath.EndsWith(".js")) {
        return realLineNumber;
    }

    ResourceCache* cache = GetSubsystem<ResourceCache>();

    String path;
    const Vector<String>& searchPaths = GetModuleSearchPaths();
    for (unsigned i = 0; i < searchPaths.Size(); i++)
    {
        String checkPath = searchPaths[i] + mapPath;

        if (cache->Exists(checkPath))
        {
            path = checkPath;
            break;
        }

    }

    if (!path.Length())
        return realLineNumber;


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
    duk_push_string(ctx_, "AtomicEditor/JavaScript/Lib/jsutils");
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

void JSVM::DumpJavascriptObjects()
{
    ATOMIC_LOGINFOF("--- JS Objects ---");
    ATOMIC_LOGINFOF("Stash Count: %u, Total Stash: %u, Total Unstash: %u", stashCount_, totalStashCount_, totalUnstashCount_);

    HashMap<StringHash, String> strLookup;
    HashMap<StringHash, unsigned> totalClassCount;
    HashMap<StringHash, unsigned> stashedClassCount;
    HashMap<StringHash, int> maxRefCount;

    StringHash refCountedTypeHash("RefCounted");
    strLookup[refCountedTypeHash] = "RefCounted";

    duk_push_global_stash(ctx_);
    duk_get_prop_index(ctx_, -1, JS_GLOBALSTASH_INDEX_REFCOUNTED_REGISTRY);

    HashMap<void*, RefCounted*>::ConstIterator itr = heapToObject_.Begin();
    while (itr != heapToObject_.End())
    {
        void* heapPtr = itr->first_;
        RefCounted* refCounted = itr->second_;

        // TODO: need a lookup for refcounted classid to typename
        const String& className = refCounted->IsObject() ? ((Object*)refCounted)->GetTypeName() : "RefCounted";
        StringHash typeHash = refCounted->IsObject() ? ((Object*)refCounted)->GetType() : refCountedTypeHash;

        strLookup.InsertNew(typeHash, className);

        totalClassCount.InsertNew(typeHash, 0);
        totalClassCount[typeHash]++;

        maxRefCount.InsertNew(typeHash, 0);
        if (refCounted->Refs() > maxRefCount[typeHash])
            maxRefCount[typeHash] = refCounted->Refs();

        duk_push_pointer(ctx_, refCounted);
        duk_get_prop(ctx_, -2);

        if (!duk_is_undefined(ctx_, -1))
        {
            stashedClassCount.InsertNew(typeHash, 0);
            stashedClassCount[typeHash]++;
        }

        duk_pop(ctx_);

        itr++;

    }

    HashMap<StringHash, String>::ConstIterator itr2 = strLookup.Begin();
    while (itr2 != strLookup.End())
    {
        StringHash typeHash = itr2->first_;
        const String& className = itr2->second_;
        unsigned totalCount = totalClassCount[typeHash];
        unsigned stashedCount = stashedClassCount[typeHash];
        int _maxRefCount = maxRefCount[typeHash];

        ATOMIC_LOGINFOF("Classname: %s, Total: %u, Stashed: %u, Max Refs: %i", className.CString(), totalCount, stashedCount, _maxRefCount);

        itr2++;
    }

    duk_pop_2(ctx_);

}

}
