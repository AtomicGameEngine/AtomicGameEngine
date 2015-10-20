
#include <Atomic/Core/StringUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Resource/ResourceCache.h>

#include "CSEventHelper.h"
#include "CSComponent.h"
#include "NETCore.h"
#include "NETManaged.h"
#include "NETAssemblyFile.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#include "Platforms/Windows/NETHostWindows.h"
#else
#include "Platforms/Unix/NETHostUnix.h"
#endif

namespace Atomic
{

/// Register NETCore library objects.
void ATOMIC_API RegisterNETCoreLibrary(Context* context);

WeakPtr<Context> NETCore::csContext_;
WeakPtr<NETCore> NETCore::instance_;

NETCore::NETCore(Context* context) :
    Object(context)
{
    RegisterNETCoreLibrary(context_);

    assert(!instance_);
    instance_ = this;
    csContext_ = context;
}

NETCore::~NETCore()
{
    Shutdown();
    context_->RemoveGlobalEventListener(context_->GetSubsystem<CSEventDispatcher>());
    context_->RemoveSubsystem(CSEventDispatcher::GetTypeStatic());
    context_->RemoveSubsystem(NETManaged::GetTypeStatic());

    instance_ = NULL;
}

void NETCore::Shutdown()
{

    RefCounted::SetRefCountedDeletedFunction(0);

    if (netHost_.NotNull())
    {
        netHost_->Shutdown();
        netHost_ = 0;
    }
}

extern "C"
{

// http://ybeernet.blogspot.com/2011/03/techniques-of-calling-unmanaged-code.html
// pinvoke is faster than [UnmanagedFunctionPointer] :/
// [SuppressUnmanagedCodeSecurity] <--- add this attribute, in any event


#ifdef ATOMIC_PLATFORM_WINDOWS
#pragma warning(disable: 4244) // possible loss of data
#define ATOMIC_EXPORT_API __declspec(dllexport)
#else
#define ATOMIC_EXPORT_API
#endif

ATOMIC_EXPORT_API ClassID csb_Atomic_RefCounted_GetClassID(RefCounted* refCounted)
{
    if (!refCounted)
        return 0;

    return refCounted->GetClassID();
}

ATOMIC_EXPORT_API RefCounted* csb_AtomicEngine_GetSubsystem(const char* name)
{
    return NETCore::GetContext()->GetSubsystem(name);
}

ATOMIC_EXPORT_API void csb_AtomicEngine_ReleaseRef(RefCounted* ref)
{
    if (!ref)
        return;

    ref->ReleaseRef();
}


}

void NETCore::WaitForDebuggerConnect()
{
    netHost_->WaitForDebuggerConnect();
}

void NETCore::AddAssemblyLoadPath(const String& assemblyPath)
{
    typedef void (*AddAssemblyLoadPathFunction)(const char* assemblyPath);
    AddAssemblyLoadPathFunction addAssemblyLoadPath;

    bool result = CreateDelegate(
                    "AtomicNETBootstrap",
                    "Atomic.Bootstrap.AtomicLoadContext",
                    "AddAssemblyLoadPath",
                    (void**) &addAssemblyLoadPath);

    if (result)
    {
        addAssemblyLoadPath(assemblyPath.CString());
    }

}

bool NETCore::CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut)
{
    return netHost_->CreateDelegate(assemblyName, qualifiedClassName, methodName, funcOut);
}

bool NETCore::Start()
{
    if (context_->GetEditorContext())
        return true;

    typedef void (*StartFunction)();
    StartFunction start;

    bool result = netHost_->CreateDelegate(
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

        result = netHost_->CreateDelegate(
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

bool NETCore::Initialize(String& errorMsg)
{

#ifdef ATOMIC_PLATFORM_WINDOWS
    netHost_ = new NETHostWindows(context_);
#else
    netHost_ = new NETHostUnix(context_);
#endif

    if (!netHost_->Initialize())
    {
        errorMsg = "NETHost Failed to Initialize";
        return false;
    }

    // MOVE THIS!
    typedef void (*StartupFunction)(const char* assemblyLoadPaths);
    StartupFunction startup;

    // The coreclr binding model will become locked upon loading the first assembly that is not on the TPA list, or
    // upon initializing the default context for the first time. For this test, test assemblies are located alongside
    // corerun, and hence will be on the TPA list. So, we should be able to set the default context once successfully,
    // and fail on the second try.

    // AssemblyLoadContext
    // https://github.com/dotnet/corefx/issues/3054
    // dnx loader
    // https://github.com/aspnet/dnx/tree/dev/src/Microsoft.Dnx.Loader

    bool result = netHost_->CreateDelegate(
                    "AtomicNETBootstrap",
                    "Atomic.Bootstrap.AtomicLoadContext",
                    "Startup",
                    (void**) &startup);

    if (result)
    {
        startup(netHost_->GetCoreCLRAssemblyLoadPaths().CString());
    }

    // MOVE THIS!
    typedef void (*InitializeFunction)();
    InitializeFunction init;

    result = netHost_->CreateDelegate(
                    "AtomicNETEngine",
                    "AtomicEngine.Atomic",
                    "Initialize",
                    (void**) &init);

    if (result)
    {
        init();
    }

    SharedPtr<NETManaged> managed(new NETManaged(context_));
    context_->RegisterSubsystem(managed);

    SharedPtr<CSEventDispatcher> dispatcher(new CSEventDispatcher(context_));
    context_->RegisterSubsystem(dispatcher);
    context_->AddGlobalEventListener(dispatcher);

    if (!context_->GetEditorContext())
    {
        SubscribeToEvent(E_UPDATE, HANDLER(NETCore, HandleUpdate));
    }

    return managed->Initialize();

}

void NETCore::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    GetSubsystem<NETManaged>()->NETUpdate(eventData[P_TIMESTEP].GetFloat());
}


void RegisterNETCoreLibrary(Context* context)
{
    NETAssemblyFile::RegisterObject(context);
    CSComponent::RegisterObject(context);
}


}

