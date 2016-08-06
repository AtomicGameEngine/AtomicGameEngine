//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
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

#include <Atomic/Core/Object.h>

#include "Subprocess.h"

using namespace Atomic;

namespace ToolCore
{

ATOMIC_EVENT(E_SUBPROCESSOUTPUT, SubprocessOutput)
{
    ATOMIC_PARAM(P_TEXT, Text);      // string
}

ATOMIC_EVENT(E_SUBPROCESSCOMPLETE, SubprocessComplete)
{
    ATOMIC_PARAM(P_PROCESSKEY, ProcessKey); // unsigned
    ATOMIC_PARAM(P_RETCODE, RetCode);      // int (return code of process)
}

class SubprocessSystem : public Object
{
    ATOMIC_OBJECT(SubprocessSystem, Object);

public:
    /// Construct.
    SubprocessSystem(Context* context);
    /// Destruct.
    virtual ~SubprocessSystem();


    Subprocess* Launch(const String& command, const Vector<String>& args, const String& initialDirectory = "");
    Subprocess* Launch(const String& command, const Vector<String>& args, const String& initialDirectory, const Poco::Process::Env& env);

private:

    void HandleUpdate(StringHash eventType, VariantMap& eventData);

    Vector<SharedPtr<Subprocess> > processes_;
    float updateTimer_;

};

}
