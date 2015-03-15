// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Object.h>
#include <Atomic/Container/List.h>

#include <Atomic/IO/Log.h>

#include "JSAPI.h"

//#define JSVM_DEBUG

namespace Atomic
{

class JSFile;

class ATOMIC_API JSVM : public Object
{
    OBJECT(JSVM);

public:
    /// Construct.
    JSVM(Context* context);
    /// Destruct.
    virtual ~JSVM();

    void InitJSContext();

    bool ExecuteFile(File* file);

    // Resources/Scripts/*.js
    bool ExecuteScript(const String& scriptPath);
    // Resources/Script/main.js

    // Catches not requiring AtomicGame, etc
    bool ExecuteMain();

    bool ExecuteFunction(const String& functionName);

    inline static JSVM* GetJSVM(duk_context* context)
    {
        return instance_;
    }

    inline duk_context* GetJSContext() { return ctx_; }

    void DumpJavascriptObjects() {}

#ifdef JSVM_DEBUG

    inline void ValidateJSHeapPtr(void* heapptr)
    {
        assert(removedHeapPtr_.Find(heapptr) == removedHeapPtr_.End());
        assert(heapToObject_.Find(heapptr) != heapToObject_.End());

    }
#endif


    inline void AddObject(void* heapptr, RefCounted* object)
    {
        assert(!object->JSGetHeapPtr());

        object->JSSetHeapPtr(heapptr);

#ifdef JSVM_DEBUG
        assert(heapToObject_.Find(heapptr) == heapToObject_.End());
#endif


        heapToObject_[heapptr] = object;

#ifdef JSVM_DEBUG
        HashMap<void*, void*>::Iterator itr = removedHeapPtr_.Find(heapptr);
        if (itr != removedHeapPtr_.End())
            removedHeapPtr_.Erase(itr);
#endif

    }

    inline void RemoveObject(RefCounted* object)
    {
        void* heapptr = object->JSGetHeapPtr();
        assert(heapptr);
        object->JSSetHeapPtr(NULL);
        HashMap<void*, RefCounted*>::Iterator hitr = heapToObject_.Find(heapptr);
        assert(hitr != heapToObject_.End());
        heapToObject_.Erase(hitr);

#ifdef JSVM_DEBUG
        assert(removedHeapPtr_.Find(heapptr) == removedHeapPtr_.End());
        removedHeapPtr_[heapptr] = heapptr;
#endif

    }

    inline RefCounted* GetObjectPtr(void* heapptr)
    {
#ifdef JSVM_DEBUG
        assert(!removedHeapPtr_.Contains(heapptr));
#endif        
        assert(heapToObject_.Contains(heapptr));

#ifdef JSVM_DEBUG
        RefCounted* ref = heapToObject_[heapptr];
        assert(ref->JSGetHeapPtr() == heapptr);
#endif
        return heapToObject_[heapptr];
    }

    void SetModuleSearchPath(const String& searchPath)
    {
        moduleSearchPath_ = searchPath;
    }

    void SetLastModuleSearchFile(const String& fileName) { lastModuleSearchFilename_ = fileName; }

    const String& GetLastModuleSearchFile() { return lastModuleSearchFilename_; }

    const String& GetErrorString() { return errorString_; }

    void SendJSErrorEvent(const String& filename = String::EMPTY);

private:

    bool GenerateComponent(const String& cname, const String& jsfilename, const String& csource);

    void InitComponents();
    void InitPackageComponents();

    void SubscribeToEvents();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    static int js_module_search(duk_context* ctx);

    duk_context* ctx_;

    HashMap<void*, RefCounted*> heapToObject_;

#ifdef JSVM_DEBUG
    // Debugging
    HashMap<void*, void*> removedHeapPtr_;

#endif

    List<Object*> jsObjects_;

    float gcTime_;

    String moduleSearchPath_;
    String lastModuleSearchFilename_;

    String errorString_;

    static JSVM* instance_;

};

template<typename T>
T* js_to_class_instance(duk_context* ctx, int index, unsigned classID)
{
    if (!duk_is_object(ctx, index))
        return NULL;

    return (T*) JSVM::GetJSVM(ctx)->GetObjectPtr(duk_get_heapptr(ctx, index));
}

// pushes null if instance is null
// pushes from object store if already wrapped
// pushes a new'd instance with wrapped native
// must be an Object (so downcast works)
inline bool js_push_class_object_instance(duk_context* ctx, const RefCounted *instance, const char* classname = "")
{
    if (!instance)
    {
        duk_push_null(ctx);
        return true;
    }

    int top = duk_get_top(ctx);

    if (instance->JSGetHeapPtr())
    {
        duk_push_heapptr(ctx, instance->JSGetHeapPtr());
        assert(duk_is_object(ctx, -1));
        return true;
    }

    duk_get_global_string(ctx, "Atomic");

    // will not handle renamed classes
    if (instance->IsObject())
        duk_get_prop_string(ctx, -1, ((Object*)instance)->GetTypeName().CString());
    else
    {
        duk_get_prop_string(ctx, -1, classname);
    }
    duk_push_pointer(ctx, (void*) instance);
    duk_new(ctx, 1);
    duk_remove(ctx, -2); // remove Atomic object

    assert(duk_is_object(ctx, -1));

    assert ((top + 1) == duk_get_top(ctx));
    return true;
}


}
