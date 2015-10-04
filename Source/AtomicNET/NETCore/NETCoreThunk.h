
#pragma once

#include "CSComponent.h"

namespace Atomic
{

typedef CSComponent* (*CSComponentCreateFunctionPtr)(const char* csComponentTypeName);
typedef void (*CSComponentCallMethodFunctionPtr)(unsigned id, CSComponentMethod method, float value);
typedef void (*CSBeginSendEventFunctionPtr)(unsigned senderRefID, unsigned eventType, VariantMap* eventData);

typedef void (*Atomic_NETManaged_SetCSComponentCreate_Function)(CSComponentCreateFunctionPtr ptr);
typedef void (*Atomic_NETManaged_SetCSComponentCallMethod_Function)(CSComponentCallMethodFunctionPtr ptr);
typedef void (*Atomic_NETManaged_SetCSBeginSendEvent_Function)(CSBeginSendEventFunctionPtr ptr);

typedef void (*Atomic_AObject_SendEvent_Function)(Object* object, const char* eventType);

typedef RefCounted* (*AtomicEngine_GetSubsystem_Function)(const char* name);
typedef unsigned (*Atomic_StringToStringHash_Function)(const char* stringValue);
typedef RefCounted* (*Atomic_VariantMap_GetInstance_Function)(VariantMap& vmap, const char* key);
typedef void (*Atomic_RefCounted_SafeAddRef_Function)(unsigned id);
typedef void (*Atomic_RefCounted_SafeReleaseRef_Function)(unsigned id);
typedef ClassID (*Atomic_RefCounted_GetClassID_Function)(RefCounted* refCounted);

typedef struct AtomicNETCoreThunk_s
{
    Atomic_NETManaged_SetCSComponentCreate_Function __Atomic_NETManaged_SetCSComponentCreate;
    Atomic_NETManaged_SetCSComponentCallMethod_Function __Atomic_NETManaged_SetCSComponentCallMethod;
    Atomic_NETManaged_SetCSBeginSendEvent_Function __Atomic_NETManaged_SetCSBeginSendEvent;

    Atomic_AObject_SendEvent_Function __Atomic_AObject_SendEvent;

    AtomicEngine_GetSubsystem_Function __AtomicEngine_GetSubsystem;
    Atomic_StringToStringHash_Function __Atomic_StringToStringHash;
    Atomic_VariantMap_GetInstance_Function __Atomic_VariantMap_GetInstance;
    Atomic_RefCounted_SafeAddRef_Function __Atomic_RefCounted_SafeAddRef;
    Atomic_RefCounted_SafeReleaseRef_Function __Atomic_RefCounted_SafeReleaseRef;
    Atomic_RefCounted_GetClassID_Function __Atomic_RefCounted_GetClassID;

} AtomicNETCoreThunk_t;

void NetCoreThunkInit();

void Atomic_NETManaged_SetCSComponentCreate(CSComponentCreateFunctionPtr method);

void Atomic_NETManaged_SetCSComponentCallMethod(CSComponentCallMethodFunctionPtr method);

void Atomic_NETManaged_SetCSBeginSendEvent(CSBeginSendEventFunctionPtr method);

void Atomic_AObject_SendEvent(Object* object, const char* eventType);

ClassID Atomic_RefCounted_GetClassID(RefCounted* refCounted);

RefCounted* AtomicEngine_GetSubsystem(const char* name);

void Atomic_RefCounted_SafeAddRef(unsigned id);

void Atomic_RefCounted_SafeReleaseRef(unsigned id);

unsigned Atomic_StringToStringHash(const char* stringValue);

// Variant Map
RefCounted* Atomic_VariantMap_GetInstance(VariantMap& vmap, const char* key);

}
