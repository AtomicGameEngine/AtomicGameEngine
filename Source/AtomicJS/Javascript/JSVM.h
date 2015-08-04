// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Object.h>
#include <Atomic/Container/List.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "JSAPI.h"
#include "JSEvents.h"

//#define JSVM_DEBUG

namespace Atomic
{

class JSFile;
class JSUI;
class JSMetrics;

class ATOMIC_API JSVM : public Object
{

    friend class JSMetrics;

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
    bool ExecuteMain();

    bool ExecuteFunction(const String& functionName);

    inline static JSVM* GetJSVM(duk_context* context)
    {
        return instance_;
    }

    inline duk_context* GetJSContext() { return ctx_; }

    void GC();
    JSMetrics* GetMetrics() { return metrics_; }

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

        if (object->IsObject())
        {
            objectAddedData_[ObjectAdded::P_OBJECT] = object;
            SendEvent(E_JSOBJECTADDED, objectAddedData_);
        }

    }

    inline void RemoveObject(RefCounted* object)
    {
        if (object->IsObject())
        {
            objectRemovedData_[ObjectRemoved::P_OBJECT] = object;
            SendEvent(E_JSOBJECTREMOVED, objectRemovedData_);
        }

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

    inline RefCounted* GetObjectPtr(void* heapptr, bool allowNull = false)
    {
#ifdef JSVM_DEBUG
        assert(!removedHeapPtr_.Contains(heapptr));
#endif
        if (allowNull && !heapToObject_.Contains(heapptr))
        {
            return NULL;
        }

        assert(heapToObject_.Contains(heapptr));

#ifdef JSVM_DEBUG
        RefCounted* ref = heapToObject_[heapptr];
        assert(ref->JSGetHeapPtr() == heapptr);
#endif
        return heapToObject_[heapptr];
    }

    void SetModuleSearchPaths(const String& searchPath)
    {
        moduleSearchPath_ = searchPath.Split(';');
        for (unsigned i = 0; i < moduleSearchPath_.Size(); i++)
        {
            moduleSearchPath_[i] = AddTrailingSlash(moduleSearchPath_[i]);
        }
    }

    const Vector<String>& GetModuleSearchPaths()
    {
        return moduleSearchPath_;
    }


    void SetLastModuleSearchFile(const String& fileName) { lastModuleSearchFilename_ = fileName; }

    const String& GetLastModuleSearchFile() { return lastModuleSearchFilename_; }

    const String& GetErrorString() { return errorString_; }

    void SendJSErrorEvent(const String& filename = String::EMPTY);

private:

    void SubscribeToEvents();
    void HandleUpdate(StringHash eventType, VariantMap& eventData);
    
    duk_context* ctx_;

    HashMap<void*, RefCounted*> heapToObject_;

#ifdef JSVM_DEBUG
    // Debugging
    HashMap<void*, void*> removedHeapPtr_;

#endif

    float gcTime_;

    Vector<String> moduleSearchPath_;
    String lastModuleSearchFilename_;

    String errorString_;

    SharedPtr<JSUI> ui_;

    VariantMap objectAddedData_;
    VariantMap objectRemovedData_;

    SharedPtr<JSMetrics> metrics_;

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

    duk_push_heap_stash(ctx);
    duk_push_pointer(ctx, (void*) instance->GetClassID());
    duk_get_prop(ctx, -2);

    // if this is tripped, means the class hasn't been registered and shouldn't be trying to push it
    assert(duk_is_object(ctx, -1));

    duk_get_prop_index(ctx, -1, 0);
    const char* package = duk_require_string(ctx, -1);
    duk_get_prop_index(ctx, -2, 1);
    const char* jclassname = duk_require_string(ctx, -1);

    duk_set_top(ctx, top);

    duk_get_global_string(ctx, package);
    duk_get_prop_string(ctx, -1, jclassname);

    duk_push_pointer(ctx, (void*) instance);
    duk_new(ctx, 1);
    duk_remove(ctx, -2); // remove Atomic object

    assert(duk_is_object(ctx, -1));

    assert ((top + 1) == duk_get_top(ctx));
    return true;
}


}
