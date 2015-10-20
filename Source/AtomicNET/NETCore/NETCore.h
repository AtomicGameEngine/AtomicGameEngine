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

class NETHost;

class ATOMIC_API NETCore : public Object
{

    OBJECT(NETCore);

public:
    /// Construct.
    NETCore(Context* context);
    /// Destruct.
    virtual ~NETCore();

    bool Initialize(String &errorMsg);
    void Shutdown();

    void AddAssemblyLoadPath(const String& assemblyPath);

    int ExecAssembly(const String& assemblyName, const Vector<String>& args);

    bool CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut);
    void WaitForDebuggerConnect();

    /// We access this directly in binding code, where there isn't a context
    /// to get a reference from
    static inline Context* GetContext() { return instance_->csContext_; }

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    SharedPtr<NETHost> netHost_;

    /// weak local context ref, so avoid recursion in static GetContext call
    static WeakPtr<Context> csContext_;

    static WeakPtr<NETCore> instance_;

};

}
