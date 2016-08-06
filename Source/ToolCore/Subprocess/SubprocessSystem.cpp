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

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>

#include "SubprocessSystem.h"

namespace ToolCore
{

SubprocessSystem::SubprocessSystem(Context* context) :
    Object(context),
    updateTimer_(0.0f)
{
    SubscribeToEvent(E_UPDATE, ATOMIC_HANDLER(SubprocessSystem, HandleUpdate));
}

SubprocessSystem::~SubprocessSystem()
{
    for (unsigned i = 0; i < processes_.Size(); i++)
        processes_[i]->Stop();

    processes_.Clear();

}

Subprocess* SubprocessSystem::Launch(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    Poco::Process::Env env;
    return Launch(command, args, initialDirectory, env);
}

Subprocess* SubprocessSystem::Launch(const String& command, const Vector<String>& args, const String& initialDirectory, const Poco::Process::Env& env)
{
    SharedPtr<Subprocess> process(new Subprocess(context_));

    if (process->Launch(GetNativePath(command), args, GetNativePath(initialDirectory), env))
    {
        processes_.Push(process);
        return process;
    }

    return 0;

}


void SubprocessSystem::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    Vector<Subprocess*> remove;

    for (unsigned i = 0; i < processes_.Size(); i++)
    {
        Subprocess* process = processes_[i];
        if (!process->Update(this))
        {
            remove.Push(process);
        }
    }

    for (unsigned i = 0; i < remove.Size(); i++)
    {
        processes_.Remove(SharedPtr<Subprocess>(remove[i]));
    }

}


}
