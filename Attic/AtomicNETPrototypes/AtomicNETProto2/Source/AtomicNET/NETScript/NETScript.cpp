
#include "../NETCore/NETCore.h"

#include "CSComponent.h"
#include "CSComponentAssembly.h"
#include "CSManaged.h"
#include "NETScript.h"

namespace Atomic
{

void RegisterNETScriptLibrary(Context* context);

NETScript::NETScript(Context* context) :
    Object(context)
{
    // NETScript subsystem should not be instantiated in editor context
    assert(!context->GetEditorContext());

    netCore_ = GetSubsystem<NETCore>();

    assert(netCore_.NotNull());
}

NETScript::~NETScript()
{
    //context_->RemoveGlobalEventListener(context_->GetSubsystem<CSEventDispatcher>());
    //context_->RemoveSubsystem(CSEventDispatcher::GetTypeStatic());

    context_->RemoveSubsystem(CSManaged::GetTypeStatic());

}

bool NETScript::ExecMainAssembly()
{
    typedef void (*StartFunction)();
    StartFunction start;

    bool result = netCore_->CreateDelegate(
                "AtomicNETEngine",
                "AtomicEngine.Atomic",
                "Start",
                (void**) &start);

    if (!result)
        return false;

    if (result)
    {
        start();

        // Load Project Assemblies

        typedef void (*ExecMainAssemblyFunction)();
        ExecMainAssemblyFunction execMainAssembly;

        result = netCore_->CreateDelegate(
                    "AtomicNETEngine",
                    "AtomicEngine.Atomic",
                    "ExecMainAssembly",
                    (void**) &execMainAssembly);

        if (!result)
            return false;

        execMainAssembly();

    }

    return true;

}

bool NETScript::Initialize()
{
    // MOVE THIS!
    typedef void (*InitializeFunction)();
    InitializeFunction init;

    bool result = netCore_->CreateDelegate(
                "AtomicNETEngine",
                "AtomicEngine.Atomic",
                "Initialize",
                (void**) &init);

    if (!result)
        return false;

    init();

    csManaged_ = new CSManaged(context_);
    context_->RegisterSubsystem(csManaged_);

    //SharedPtr<CSEventDispatcher> dispatcher(new CSEventDispatcher(context_));
    //context_->RegisterSubsystem(dispatcher);
    //context_->AddGlobalEventListener(dispatcher);

    //if (!context_->GetEditorContext())
    //{
    //SubscribeToEvent(E_UPDATE, HANDLER(NETCore, HandleUpdate));
    //}

    return csManaged_->Initialize();

}

void RegisterNETScriptLibrary(Context* context)
{
    CSComponentAssembly::RegisterObject(context);
    CSComponent::RegisterObject(context);
}

}
