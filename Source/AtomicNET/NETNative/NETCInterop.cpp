#include <Atomic/Script/ScriptVariantMap.h>
#include <Atomic/IPC/IPC.h>

#include "NETCore.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#define ATOMIC_EXPORT_API __declspec(dllexport)
#else
#define ATOMIC_EXPORT_API
#endif

namespace Atomic
{

    extern "C"
    {

        ATOMIC_EXPORT_API ClassID csb_Atomic_RefCounted_GetClassID(RefCounted* refCounted)
        {
            if (!refCounted)
                return 0;

            return refCounted->GetClassID();
        }

        ATOMIC_EXPORT_API void csb_Atomic_AObject_SendEvent(Object* obj, const char* eventType, ScriptVariantMap* vmap)
        {
            obj->SendEvent(eventType, vmap ? vmap->GetVariantMap() : obj->GetEventDataMap());
        }

        ATOMIC_EXPORT_API ClassID csb_Atomic_NETCore_Initialize(NETCoreDelegates* delegates)
        {
            Context* context = new Context();
            NETCore* netCore = new NETCore(context, delegates);
            context->RegisterSubsystem(netCore);
            return netCore;
        }

        ATOMIC_EXPORT_API unsigned csb_Atomic_AtomicNET_StringToStringHash(const char* str)
        {
            unsigned hash = 0;

            if (!str)
                return hash;

            while (*str)
            {
                // Perform the actual hashing as case-insensitive
                char c = *str;
                hash = SDBMHash(hash, (unsigned char)tolower(c));
                ++str;
            }

            return hash;
        }

        ATOMIC_EXPORT_API void csb_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(ScriptVariantMap* svm, VariantMap* vm)
        {
            if (!svm)
                return;

            if (!vm)
            {
                svm->CopySourceVariantMap(Variant::emptyVariantMap);
                return;
            }

            svm->CopySourceVariantMap(*vm);

        }

        ATOMIC_EXPORT_API void* csb_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(ScriptVariantMap* svm, const char* key)
        {
            if (!svm || !key || !strlen(key))
                return nullptr;

            return svm->GetVoidPtr(key);

        }

        // IPC
        ATOMIC_EXPORT_API void csb_Atomic_IPC_SendEventToBrokerWithEventData(IPC* ipc, const char* eventType, ScriptVariantMap* variantMap)
        {
            if (variantMap)
                ipc->SendEventToBroker(eventType, variantMap->GetVariantMap());
            else
                ipc->SendEventToBroker(eventType);

        }

    }
}



