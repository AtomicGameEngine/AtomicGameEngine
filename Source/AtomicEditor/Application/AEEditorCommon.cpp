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

#include <Atomic/Script/ScriptSystem.h>
#include <AtomicJS/Javascript/Javascript.h>

#include <ToolCore/ToolSystem.h>
#include <ToolCore/ToolEnvironment.h>

#ifdef ATOMIC_DOTNET
#include <AtomicNET/NETCore/NETHost.h>
#include <AtomicNET/NETCore/NETCore.h>
#include <AtomicNET/NETScript/NETScript.h>
#endif

#include "AEEditorCommon.h"

namespace Atomic
{
    void jsapi_init_atomicnet(JSVM* vm);
}

using namespace ToolCore;

namespace ToolCore
{
    extern void jsapi_init_toolcore(JSVM* vm);
}


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

    Javascript* javascript = GetSubsystem<Javascript>();
    vm_ = javascript->InstantiateVM("MainVM");
    vm_->InitJSContext();

    jsapi_init_toolcore(vm_);

#ifdef ATOMIC_DOTNET
    jsapi_init_atomicnet(vm_);
#endif

}

void AEEditorCommon::Setup()
{

#ifdef ATOMIC_3D
    RegisterEnvironmentLibrary(context_);
#endif

#ifdef ATOMIC_DOTNET
    RegisterNETScriptLibrary(context_);
#endif

    // Register IPC system
    context_->RegisterSubsystem(new IPC(context_));

    context_->RegisterSubsystem(new ScriptSystem(context_));

    // Instantiate and register the Javascript subsystem
    Javascript* javascript = new Javascript(context_);
    context_->RegisterSubsystem(javascript);

    ToolEnvironment* env = new ToolEnvironment(context_);
    context_->RegisterSubsystem(env);

#ifdef ATOMIC_DEV_BUILD

    if (!env->InitFromJSON())
    {
        ErrorExit(ToString("Unable to initialize tool environment from %s", env->GetDevConfigFilename().CString()));
        return;
    }
#else

    env->InitFromPackage();

#endif

#ifdef ATOMIC_DOTNET

    // Instantiate and register the AtomicNET subsystem
    SharedPtr<NETCore> netCore (new NETCore(context_));
    context_->RegisterSubsystem(netCore);
    String netCoreErrorMsg;

    NETHost::SetCoreCLRFilesAbsPath(env->GetNETCoreCLRAbsPath());
    NETHost::SetCoreCLRTPAPaths(env->GetNETTPAPaths());
    NETHost::SetCoreCLRAssemblyLoadPaths(env->GetNETAssemblyLoadPaths());

    if (!netCore->Initialize(netCoreErrorMsg))
    {
        LOGERRORF("NetCore: Unable to initialize! %s", netCoreErrorMsg.CString());
        context_->RemoveSubsystem(NETCore::GetTypeStatic());
    }
    else
    {

    }
#endif

    ToolSystem* system = new ToolSystem(context_);
    context_->RegisterSubsystem(system);

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
