
#include "NETCore.h"
#include "NETManaged.h"
#include "NETCoreThunk.h"


namespace Atomic
{

static AtomicNETCoreThunk_t AtomicNETCoreThunk;

void Atomic_NETManaged_SetCSComponentCreate(CSComponentCreateFunctionPtr method)
{
    NETCore::GetContext()->GetSubsystem<NETManaged>()->SetCSComponentCreate(method);
}


void Atomic_NETManaged_SetCSComponentCallMethod(CSComponentCallMethodFunctionPtr method)
{
    NETCore::GetContext()->GetSubsystem<NETManaged>()->SetCSComponentCallMethod(method);
}

// Event Handling

void Atomic_NETManaged_SetCSBeginSendEvent(CSBeginSendEventFunctionPtr method)
{
    NETCore::GetContext()->GetSubsystem<NETManaged>()->SetCSBeginSendEvent(method);
}

void Atomic_AObject_SendEvent(Object* object, const char* eventType)
{
    object->SendEvent(eventType);
}

ClassID Atomic_RefCounted_GetClassID(RefCounted* refCounted)
{
    if (!refCounted)
        return 0;

    return refCounted->GetClassID();
}

RefCounted* AtomicEngine_GetSubsystem(const char* name)
{
    return NETCore::GetContext()->GetSubsystem(name);
}

void Atomic_RefCounted_SafeAddRef(unsigned id)
{
    RefCounted* ref = RefCounted::GetByID(id);
    if (ref)
        ref->AddRef();

}

void Atomic_RefCounted_SafeReleaseRef(unsigned id)
{
    RefCounted* ref = RefCounted::GetByID(id);
    if (ref)
        ref->ReleaseRef();
}

unsigned Atomic_StringToStringHash(const char* stringValue)
{
    StringHash hash = stringValue;
    return hash.Value();
}

// Variant Map
RefCounted* Atomic_VariantMap_GetInstance(VariantMap& vmap, const char* key)
{
    return vmap[key].GetPtr();
}

void NetCoreThunkInit()
{
    AtomicNETCoreThunk.__Atomic_NETManaged_SetCSBeginSendEvent = Atomic_NETManaged_SetCSBeginSendEvent;
    AtomicNETCoreThunk.__Atomic_NETManaged_SetCSComponentCallMethod = Atomic_NETManaged_SetCSComponentCallMethod;
    AtomicNETCoreThunk.__Atomic_NETManaged_SetCSComponentCreate = Atomic_NETManaged_SetCSComponentCreate;

    AtomicNETCoreThunk.__Atomic_AObject_SendEvent = Atomic_AObject_SendEvent;

    AtomicNETCoreThunk.__AtomicEngine_GetSubsystem = AtomicEngine_GetSubsystem;
    AtomicNETCoreThunk.__Atomic_StringToStringHash = Atomic_StringToStringHash;
    AtomicNETCoreThunk.__Atomic_VariantMap_GetInstance = Atomic_VariantMap_GetInstance;
    AtomicNETCoreThunk.__Atomic_RefCounted_SafeAddRef = Atomic_RefCounted_SafeAddRef;
    AtomicNETCoreThunk.__Atomic_RefCounted_SafeReleaseRef = Atomic_RefCounted_SafeReleaseRef;
    AtomicNETCoreThunk.__Atomic_RefCounted_GetClassID = Atomic_RefCounted_GetClassID;
}

}
