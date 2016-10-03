
#include <Atomic/Math/MathDefs.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Script/ScriptVariantMap.h>

#include "NETCore.h"
#include "NETEventDispatcher.h"

namespace Atomic
{

SharedPtr<Context> NETCore::csContext_;
NETCoreEventDispatchFunction NETCore::eventDispatch_ = nullptr;
NETCoreUpdateDispatchFunction NETCore::updateDispatch_ = nullptr;
NETCoreRefCountedDeletedFunction NETCore::refCountedDeleted_ = nullptr;

NETCore::NETCore(Context* context, NETCoreDelegates* delegates) :
    Object(context)
{
    assert (!csContext_);
    csContext_ = context;

    eventDispatch_ = delegates->eventDispatch;
    updateDispatch_ = delegates->updateDispatch;
    refCountedDeleted_ = delegates->refCountedDeleted;

    NETEventDispatcher* dispatcher = new NETEventDispatcher(context_);
    context_->RegisterSubsystem(dispatcher);
    context_->AddGlobalEventListener(dispatcher);

    RefCounted::AddRefCountedDeletedFunction(OnRefCountedDeleted);
}

NETCore::~NETCore()
{
    RefCounted::RemoveRefCountedDeletedFunction(OnRefCountedDeleted);
    assert (!csContext_);    
}

void NETCore::OnRefCountedDeleted(RefCounted* ref)
{
    if (csContext_.Null())
        return;

    if (refCountedDeleted_)
        refCountedDeleted_(ref);

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

