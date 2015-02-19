// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>

#include "AEEvents.h"

#include "AESubprocessSystem.h"

namespace AtomicEditor
{

SubprocessSystem::SubprocessSystem(Context* context) :
    Object(context),
    updateTimer_(0.0f)
{
    SubscribeToEvent(E_UPDATE, HANDLER(SubprocessSystem, HandleUpdate));
    SubscribeToEvent(E_EDITORSHUTDOWN, HANDLER(SubprocessSystem, HandleEditorShutdown));
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

void SubprocessSystem::HandleEditorShutdown(StringHash eventType, VariantMap& eventData)
{
    context_->RemoveSubsystem(GetType());
}

}

