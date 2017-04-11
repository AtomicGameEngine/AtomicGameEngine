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

#pragma once

#include <Atomic/Core/Context.h>
#include <Atomic/Core/Object.h>

namespace Atomic
{

typedef void (*NETCoreEventDispatchFunction)(Object* refCounted, unsigned eventID, VariantMap* eventData);
typedef void (*NETCoreUpdateDispatchFunction)(float timeStep);
typedef void (*NETCoreRefCountedDeletedFunction)(RefCounted* refCounted);
typedef void(*NETCoreThrowManagedExceptionFunction)(const char* errorMsg);

struct NETCoreDelegates
{
    NETCoreEventDispatchFunction eventDispatch;
    NETCoreUpdateDispatchFunction updateDispatch;
    NETCoreRefCountedDeletedFunction refCountedDeleted;
    NETCoreThrowManagedExceptionFunction throwManagedException;
};

class ATOMIC_API NETCore : public Object
{

    ATOMIC_OBJECT(NETCore, Object)

public:

    /// Construct.
    NETCore(Context* context, NETCoreDelegates* delegates);

    /// Destruct.
    virtual ~NETCore();

    static void Shutdown();

    static bool EnsureMainThread(const String& throwMsg);

    static void RegisterNETEventType(unsigned eventType);

    inline static void DispatchEvent(Object* refCounted, unsigned eventID, VariantMap* eventData = nullptr) { eventDispatch_(refCounted, eventID, eventData); }
    inline static void DispatchUpdateEvent(float timeStep) { if (updateDispatch_) updateDispatch_(timeStep); }

    /// Throws a managed exception in managed code from native code
    inline static void ThrowManagedException(const String& errorMsg) { if (throwManagedException_) throwManagedException_(errorMsg.CString()); }

    /// We access this directly in binding code, where there isn't a context
    /// to get a reference from
    static inline Context* GetContext() { return csContext_; }

private:

    static void OnRefCountedDeleted(RefCounted* ref);

    static SharedPtr<Context> csContext_;

    static NETCoreUpdateDispatchFunction updateDispatch_;
    static NETCoreEventDispatchFunction eventDispatch_;
    static NETCoreRefCountedDeletedFunction refCountedDeleted_;
    static NETCoreThrowManagedExceptionFunction throwManagedException_;

};

}
