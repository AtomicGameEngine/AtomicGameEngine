
#include <Atomic/Core/StringUtils.h>
#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Resource/ResourceCache.h>

#include "NETCore.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#include "Platforms/Windows/NETHostWindows.h"
#else
#include "Platforms/Unix/NETHostUnix.h"
#endif

namespace Atomic
{

WeakPtr<Context> NETCore::csContext_;
WeakPtr<NETCore> NETCore::instance_;

NETCore::NETCore(Context* context) :
    Object(context)
{
    assert(!instance_);
    instance_ = this;
    csContext_ = context;
}

NETCore::~NETCore()
{
    Shutdown();
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

int NETCore::ExecAssembly(const String& assemblyName, const Vector<String>& args)
{
    return netHost_->ExecAssembly(assemblyName, args);
}

bool NETCore::CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut)
{
    return netHost_->CreateDelegate(assemblyName, qualifiedClassName, methodName, funcOut);
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

    if (!result)
        return false;

    startup(netHost_->GetCoreCLRAssemblyLoadPaths().CString());

    return true;

}

void NETCore::HandleUpdate(StringHash eventType, VariantMap& eventData)
{
    using namespace Update;

    //GetSubsystem<NETManaged>()->NETUpdate(eventData[P_TIMESTEP].GetFloat());
}


}

