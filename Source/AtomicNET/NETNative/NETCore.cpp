
#include <Atomic/Math/MathDefs.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Script/ScriptVariantMap.h>

#include "NETCore.h"
#include "NETEventDispatcher.h"

namespace Atomic
{


SharedPtr<Context> NETCore::csContext_;
NETCoreEventDispatchFunction NETCore::eventDispatch_ = nullptr;
NETCoreUpdateDispatchFunction NETCore::updateDispatch_ = nullptr;

NETCore::NETCore(Context* context, NETCoreDelegates* delegates) :
    Object(context)
{
    assert (!csContext_);
    csContext_ = context;

    eventDispatch_ = delegates->eventDispatch;
    updateDispatch_ = delegates->updateDispatch;

    NETEventDispatcher* dispatcher = new NETEventDispatcher(context_);
    context_->RegisterSubsystem(dispatcher);
    context_->AddGlobalEventListener(dispatcher);
}

NETCore::~NETCore()
{
    assert (!csContext_);
}

void NETCore::RegisterNETEventType(unsigned eventType)
{
    NETEventDispatcher* dispatcher = csContext_->GetSubsystem<NETEventDispatcher>();
    dispatcher->RegisterNETEvent(StringHash(eventType));
}

void NETCore::Shutdown()
{
    assert (csContext_);

    csContext_->RemoveGlobalEventListener(csContext_->GetSubsystem<NETEventDispatcher>());
    csContext_->RemoveSubsystem(NETEventDispatcher::GetTypeStatic());

    eventDispatch_ = nullptr;
    csContext_ = nullptr;
}

}

