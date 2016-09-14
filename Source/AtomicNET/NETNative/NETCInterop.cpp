
#include <Atomic/Script/ScriptVariantMap.h>
#include <Atomic/IPC/IPC.h>

#include <Atomic/Graphics/VertexBuffer.h>
#include <Atomic/Graphics/Viewport.h>
#include <Atomic/Graphics/Graphics.h>


#include "NETCore.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#define ATOMIC_EXPORT_API __declspec(dllexport)
#else
#define ATOMIC_EXPORT_API
#endif

// TODO: Split into separate module files

// IMPORTANT: methods here are prefaced with csi_ instead of csb_, the later being automatically generated bindings

namespace Atomic
{

#ifdef ATOMIC_PLATFORM_IOS
    static const char *sdlResourceDir = 0;
    static const char *sdlDocumentsDir = 0;
#endif

    extern "C"
    {

        ATOMIC_EXPORT_API ClassID csi_Atomic_RefCounted_GetClassID(RefCounted* refCounted)
        {
            if (!refCounted)
                return 0;

            return refCounted->GetClassID();
        }

        ATOMIC_EXPORT_API void csi_AtomicEngine_ReleaseRef(RefCounted* refCounted)
        {
            if (!refCounted)
                return;

            refCounted->ReleaseRef();
        }

        ATOMIC_EXPORT_API const char* csi_Atomic_AObject_GetTypeName(Object* self)
        {

           static String returnValue;
           returnValue = self->GetTypeName();
           return returnValue.CString();
        }

        ATOMIC_EXPORT_API int csi_Atomic_RefCounted_Refs(RefCounted* self)
        {
           return self->Refs();
        }


        ATOMIC_EXPORT_API void csi_Atomic_AObject_SendEvent(Object* obj, const char* eventType, ScriptVariantMap* vmap)
        {
            obj->SendEvent(eventType, vmap ? vmap->GetVariantMap() : obj->GetEventDataMap());
        }

        ATOMIC_EXPORT_API ClassID csi_Atomic_NETCore_Initialize(NETCoreEventDispatchFunction eventDispatch, NETCoreUpdateDispatchFunction updateDispatch)
        {
            Context* context = new Context();

            // NOTE: We don't simply marshal the NETCoreDelegates structure due to iOS "reverse callback" limitation
            NETCoreDelegates delegates;
            delegates.eventDispatch = eventDispatch;
            delegates.updateDispatch = updateDispatch;

            NETCore* netCore = new NETCore(context, &delegates);
            context->RegisterSubsystem(netCore);
            return netCore;
        }

        ATOMIC_EXPORT_API unsigned csi_Atomic_AtomicNET_StringToStringHash(const char* str)
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

        ATOMIC_EXPORT_API void csi_Atomic_AtomicNET_ScriptVariantMapCopyVariantMap(ScriptVariantMap* svm, VariantMap* vm)
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

        ATOMIC_EXPORT_API void* csi_Atomic_AtomicNET_ScriptVariantMap_GetVoidPtr(ScriptVariantMap* svm, const char* key)
        {
            if (!svm || !key || !strlen(key))
                return nullptr;

            return svm->GetVoidPtr(key);

        }

        // IPC
        ATOMIC_EXPORT_API void csi_Atomic_IPC_SendEventToBrokerWithEventData(IPC* ipc, const char* eventType, ScriptVariantMap* variantMap)
        {

#ifdef ATOMIC_PLATFORM_DESKTOP
            if (variantMap)
                ipc->SendEventToBroker(eventType, variantMap->GetVariantMap());
            else
                ipc->SendEventToBroker(eventType);
#endif

        }

        // Graphics

        ATOMIC_EXPORT_API void* csi_Atomic_Graphics_GetSDLWindow()
        {
            if (!NETCore::GetContext())
                return 0;

            if (!NETCore::GetContext()->GetSubsystem<Graphics>())
                return 0;

            return NETCore::GetContext()->GetSubsystem<Graphics>()->GetSDLWindow();

        }

        ATOMIC_EXPORT_API void* csi_Atomic_VertexBuffer_Lock(VertexBuffer* vb , unsigned start, unsigned count, bool discard)
        {
            if (!vb)
                return nullptr;

            return vb->Lock(start, count, discard);

        }

        ATOMIC_EXPORT_API void csi_Atomic_Graphics_SetShaderParameter_Matrix3x4(Graphics* graphics, const char* param, Matrix3x4* matrix)
        {
            if (!graphics || !param || !strlen(param))
                return;

            graphics->SetShaderParameter(param, *matrix);
        }

#ifdef ATOMIC_PLATFORM_IOS
        ATOMIC_EXPORT_API void SDL_IOS_Init(const char *resourceDir, const char *documentsDir)
        {
            sdlResourceDir = resourceDir;
            sdlDocumentsDir = documentsDir;
        }
#endif


    }
}

#ifdef ATOMIC_PLATFORM_IOS

//FileSystem.cpp uses these functions as external.
const char* SDL_IOS_GetResourceDir()
{
    return Atomic::sdlResourceDir;
}

const char* SDL_IOS_GetDocumentsDir()
{
    return Atomic::sdlDocumentsDir;
}

#endif






