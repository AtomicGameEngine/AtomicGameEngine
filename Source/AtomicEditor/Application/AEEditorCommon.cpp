//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Engine/Engine.h>
#include <Atomic/Input/Input.h>

#include <Atomic/IPC/IPC.h>

// Move me to Engine
#include <Atomic/Environment/Environment.h>

#include <AtomicJS/Javascript/Javascript.h>

#ifdef ATOMIC_DOTNET
#include <AtomicNET/NETCore/NETCore.h>
#endif

#include "AEEditorCommon.h"

namespace AtomicEditor
{

AEEditorCommon::AEEditorCommon(Context* context) :
    Application(context)
{

}

void AEEditorCommon::Start()
{
    Input* input = GetSubsystem<Input>();
    input->SetMouseVisible(true);

    // Register IPC system
    context_->RegisterSubsystem(new IPC(context_));

    // Instantiate and register the Javascript subsystem
    Javascript* javascript = new Javascript(context_);
    context_->RegisterSubsystem(javascript);

#ifdef ATOMIC_DOTNET
    // Instantiate and register the AtomicNET subsystem
    SharedPtr<NETCore> netCore (new NETCore(context_));
    String netCoreErrorMsg;
    if (!netCore->Initialize("/Users/josh/Desktop/OSX.x64.Debug/", netCoreErrorMsg))
    {
        LOGERRORF("NetCore: Unable to initialize! %s", netCoreErrorMsg.CString());
    }
    else
    {
        context_->RegisterSubsystem(netCore);
    }
#endif


    vm_ = javascript->InstantiateVM("MainVM");
    vm_->InitJSContext();

}

void AEEditorCommon::Setup()
{

#ifdef ATOMIC_3D
    RegisterEnvironmentLibrary(context_);
#endif

}

void AEEditorCommon::Stop()
{

    context_->RemoveSubsystem<IPC>();

    vm_ = 0;
    context_->RemoveSubsystem<Javascript>();
    // make sure JSVM is really down and no outstanding refs
    // as if not, will hold on engine subsystems, which is bad
    assert(!JSVM::GetJSVM(0));

#ifdef ATOMIC_DOTNET
    NETCore* netCore = GetSubsystem<NETCore>();
    if (netCore)
    {
        netCore->Shutdown();
        context_->RemoveSubsystem<NETCore>();
    }
#endif
}

}
