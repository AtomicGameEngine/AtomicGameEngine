
#include <Atomic/Math/MathDefs.h>
#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/Core/Thread.h>
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
NETCoreThrowManagedExceptionFunction NETCore::throwManagedException_ = nullptr;

NETCore::NETCore(Context* context, NETCoreDelegates* delegates) :
    Object(context)
{
    assert (!csContext_);
    csContext_ = context;

    eventDispatch_ = delegates->eventDispatch;
    updateDispatch_ = delegates->updateDispatch;
    refCountedDeleted_ = delegates->refCountedDeleted;
    throwManagedException_ = delegates->throwManagedException;

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

bool NETCore::EnsureMainThread(const String& throwMsg)
{
    if (!Thread::IsMainThread())
    {
        if (throwMsg.Length())
        {
            NETCore::ThrowManagedException(throwMsg.CString());
        }

        return false;
    }

    return true;
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

    eventDispatch_ = nullptr;
    updateDispatch_ = nullptr;
    refCountedDeleted_ = nullptr;
    throwManagedException_ = nullptr;

}

}

