
#include <ThirdParty/SDL/include/SDL.h>

#include <Atomic/Core/CoreEvents.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/StringUtils.h>

#include "CSEventHelper.h"
#include "CSComponent.h"
#include "NETCore.h"
#include "NETManaged.h"
#include "NETAssemblyFile.h"

#ifdef ATOMIC_PLATFORM_WINDOWS
#include "Platforms/Windows/NETHostWindows.h"
#else
#endif

namespace Atomic
{

/*
mcs /nostdlib /noconfig /r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll HelloWorld.cs

export PAL_DBG_CHANNELS="+all.all"
*/


#ifdef ATOMIC_PLATFORM_OSX
static const char * const sCoreClrDll = "libcoreclr.dylib";
#elif ATOMIC_PLATFORM_WINDOWS
static const char * const sCoreClrDll = "coreclr.dll";
#else
static const char * const sCoreClrDll = "libcoreclr.so";
#endif

// Prototype of the coreclr_initialize function from the libcoreclr.so
typedef int (*InitializeCoreCLRFunction)(
            const char* exePath,
            const char* appDomainFriendlyName,
            int propertyCount,
            const char** propertyKeys,
            const char** propertyValues,
            void** hostHandle,
            unsigned int* domainId);

// Prototype of the coreclr_shutdown function from the libcoreclr.so
typedef int (*ShutdownCoreCLRFunction)(
            void* hostHandle,
            unsigned int domainId);

// Prototype of the coreclr_execute_assembly function from the libcoreclr.so
typedef int (*ExecuteAssemblyFunction)(
            void* hostHandle,
            unsigned int domainId,
            int argc,
            const char** argv,
            const char* managedAssemblyPath,
            unsigned int* exitCode);

typedef int (*CreateDelegateFunction)(
            void* hostHandle,
            unsigned int domainId,
            const char* entryPointAssemblyName,
            const char* entryPointTypeName,
            const char* entryPointMethodName,
            void** delegate);

static InitializeCoreCLRFunction sInitializeCoreCLR = 0;
static ExecuteAssemblyFunction sExecuteAssembly = 0;
static CreateDelegateFunction sCreateDelegate = 0;
static ShutdownCoreCLRFunction sShutdownCoreCLR = 0;

/// Register NETCore library objects.
void ATOMIC_API RegisterNETCoreLibrary(Context* context);

WeakPtr<Context> NETCore::csContext_;
WeakPtr<NETCore> NETCore::instance_;

NETCore::NETCore(Context* context) :
    Object(context),
    coreCLRDLLHandle_(0),
    hostHandle_(0),
    domainId_(0)
{
    RegisterNETCoreLibrary(context_);

    assert(!instance_);
    instance_ = this;
    csContext_ = context;
}

NETCore::~NETCore()
{
    context_->RemoveGlobalEventListener(context_->GetSubsystem<CSEventDispatcher>());
    context_->RemoveSubsystem(CSEventDispatcher::GetTypeStatic());
    context_->RemoveSubsystem(NETManaged::GetTypeStatic());

    instance_ = NULL;
}

void NETCore::GenerateTPAList(String& tpaList)
{
    tpaList = String::EMPTY;

    FileSystem* fs = GetSubsystem<FileSystem>();
    Vector<String> results;
    fs->ScanDir(results, coreCLRFilesAbsPath_, "*.dll", SCAN_FILES, true);

    Vector<String> trustedAssemblies;

    for (unsigned i = 0; i < results.Size(); i++)
    {
        const String& assembly = results[i];

        // TODO: apply filtering if necessary
        trustedAssemblies.Push(coreCLRFilesAbsPath_ + assembly);
    }

    // trustedAssemblies.Push(coreCLRFilesAbsPath_ + "HelloWorld.exe");

    tpaList.Join(trustedAssemblies, ":");

    // LOGINFOF("NetCore:: TPALIST - %s", tpaList.CString());

}

void NETCore::Shutdown()
{
    RefCounted::SetRefCountedDeletedFunction(0);

    if (sShutdownCoreCLR && hostHandle_)
    {
        sShutdownCoreCLR(hostHandle_, domainId_);
    }

    if (coreCLRDLLHandle_)
    {
        SDL_UnloadObject(coreCLRDLLHandle_);
    }

    coreCLRDLLHandle_ = 0;
    hostHandle_ = 0;
    domainId_ = 0;
    sInitializeCoreCLR = 0;
    sExecuteAssembly = 0;
    sCreateDelegate = 0;
    sShutdownCoreCLR = 0;

}

bool NETCore::InitCoreCLRDLL(String& errorMsg)
{
    String coreClrDllPath = AddTrailingSlash(coreCLRFilesAbsPath_) + sCoreClrDll;

    coreCLRDLLHandle_ = SDL_LoadObject(coreClrDllPath.CString());

    if (coreCLRDLLHandle_ == NULL)
    {
        errorMsg = ToString("NETCore: Unable to load %s with error %s",
            coreClrDllPath.CString(),
            SDL_GetError());
        return false;
    }

    sInitializeCoreCLR = (InitializeCoreCLRFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_initialize");

    if (!sInitializeCoreCLR)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_initialize entry point in %s", coreClrDllPath.CString());
        return false;
    }

    sShutdownCoreCLR = (ShutdownCoreCLRFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_shutdown");

    if (!sShutdownCoreCLR)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_shutdown entry point in %s", coreClrDllPath.CString());
        return false;
    }

    sCreateDelegate = (CreateDelegateFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_create_delegate");

    if (!sCreateDelegate)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_create_delegate entry point in %s", coreClrDllPath.CString());
        return false;
    }

    sExecuteAssembly = (ExecuteAssemblyFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_execute_assembly");

    if (!sExecuteAssembly)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_execute_assembly entry point in %s", coreClrDllPath.CString());
        return false;
    }

    return true;
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

    /*
    if (!sCreateDelegate)
        return false;

    *funcOut = 0;

    // TODO: wrap in SharedPtr to control delegate lifetime?
    int st = sCreateDelegate(hostHandle_,
                    domainId_,
                    assemblyName.CString(),
                    qualifiedClassName.CString(),
                    methodName.CString(),
                    funcOut);

    // ensure ptr isn't uninitialized
    if (st < 0)
        *funcOut = 0;

    return st >= 0;
    */

}

bool NETCore::Initialize(const String &coreCLRFilesAbsPath, const String &assemblyLoadPaths, String& errorMsg)
{
    coreCLRFilesAbsPath_ = AddTrailingSlash(coreCLRFilesAbsPath);

#ifdef ATOMIC_PLATFORM_WINDOWS
    netHost_ = new NETHostWindows(context_);
#else
#endif

    netHost_->Initialize(coreCLRFilesAbsPath_, assemblyLoadPaths);

    SharedPtr<NETManaged> managed(new NETManaged(context_));
    context_->RegisterSubsystem(managed);

    SharedPtr<CSEventDispatcher> dispatcher(new CSEventDispatcher(context_));
    context_->RegisterSubsystem(dispatcher);
    context_->AddGlobalEventListener(dispatcher);

    if (!context_->GetEditorContext())
    {
        SubscribeToEvent(E_UPDATE, HANDLER(NETCore, HandleUpdate));
    }

    managed->Initialize();


#ifdef disabled

    if (!InitCoreCLRDLL(errorMsg))
    {
        Shutdown();
        return false;
    }

    // Allowed property names:
    // APPBASE
    // - The base path of the application from which the exe and other assemblies will be loaded
    //
    // TRUSTED_PLATFORM_ASSEMBLIES
    // - The list of complete paths to each of the fully trusted assemblies
    //
    // APP_PATHS
    // - The list of paths which will be probed by the assembly loader
    //
    // APP_NI_PATHS
    // - The list of additional paths that the assembly loader will probe for ngen images
    //
    // NATIVE_DLL_SEARCH_DIRECTORIES
    // - The list of paths that will be probed for native DLLs called by PInvoke
    //

    const char *propertyKeys[] = {
        "TRUSTED_PLATFORM_ASSEMBLIES",
        "APP_PATHS",
        "APP_NI_PATHS",
        "NATIVE_DLL_SEARCH_DIRECTORIES",
        "AppDomainCompatSwitch"
    };

    String tpaList;
    GenerateTPAList(tpaList);

    String appPath = coreCLRFilesAbsPath_;// "/Users/josh/Desktop/";
    Vector<String> nativeSearch;
    nativeSearch.Push(coreCLRFilesAbsPath_);
    //nativeSearch.Push("/Users/josh/Dev/atomic/AtomicGameEngine-build/Source/AtomicNET/NETNative");
    //nativeSearch.Push("/Users/josh/Dev/atomic/AtomicGameEngine-build/Source/AtomicEditor/AtomicEditor.app/Contents/MacOS");

    String nativeDllSearchDirs;
    nativeDllSearchDirs.Join(nativeSearch, ":");

    const char *propertyValues[] = {
        // TRUSTED_PLATFORM_ASSEMBLIES
        tpaList.CString(),
        // APP_PATHS
        appPath.CString(),
        // APP_NI_PATHS
        appPath.CString(),
        // NATIVE_DLL_SEARCH_DIRECTORIES
        nativeDllSearchDirs.CString(),
        // AppDomainCompatSwitch
        "UseLatestBehaviorWhenTFMNotSpecified"
    };

    int st = sInitializeCoreCLR(
                "AtomicEditor.exe",
                "NETCore",
                sizeof(propertyKeys) / sizeof(propertyKeys[0]),
                propertyKeys,
                propertyValues,
                &hostHandle_,
                &domainId_);

    if (st < 0)
    {
        Shutdown();
        errorMsg = ToString("NETCore: coreclr_initialize failed - status: 0x%08x\n", st);
        return false;
    }

    typedef void (*StartupFunction)();
    StartupFunction startup;

    // The coreclr binding model will become locked upon loading the first assembly that is not on the TPA list, or
    // upon initializing the default context for the first time. For this test, test assemblies are located alongside
    // corerun, and hence will be on the TPA list. So, we should be able to set the default context once successfully,
    // and fail on the second try.    

    // AssemblyLoadContext
    // https://github.com/dotnet/corefx/issues/3054
    // dnx loader
    // https://github.com/aspnet/dnx/tree/dev/src/Microsoft.Dnx.Loader

    st = sCreateDelegate(hostHandle_,
                    domainId_,
                    "AtomicNETBootstrap",
                    "Atomic.Bootstrap.AtomicLoadContext",
                    "Startup",
                    (void**) &startup);

    if (st >= 0)
    {
        startup();
    }

    st = sCreateDelegate(hostHandle_,
                    domainId_,
                    "AtomicNETEngine",
                    "AtomicEngine.Atomic",
                    "Initialize",
                    (void**) &startup);

    if (st >= 0)
    {
        startup();
    }

    /*


    RefCountedDeletedFunction rcdFunction;

    st = sCreateDelegate(hostHandle_,
                    domainId_,
                    "AtomicNETEngine",
                    "AtomicEngine.NativeCore",
                    "RefCountedDeleted",
                    (void**) &rcdFunction);

    if (st >= 0)
    {
        RefCounted::SetRefCountedDeletedFunction(rcdFunction);
    }

    NETUpdateFunctionPtr updateFunction;

    st = sCreateDelegate(hostHandle_,
                    domainId_,
                    "AtomicNETEngine",
                    "AtomicEngine.NativeCore",
                    "NETUpdate",
                    (void**) &updateFunction);

    if (st >= 0)
    {
        GetSubsystem<NETManaged>()->SetNETUpdate(updateFunction);
    }

    typedef void (*InpectAssemblyFuctionPtr)(const char* path);

    InpectAssemblyFuctionPtr inspectAssembly;

    st = sCreateDelegate(hostHandle_,
                    domainId_,
                    "AtomicEditor",
                    "AtomicEditor.AtomicEditor",
                    "InspectAssembly",
                    (void**) &inspectAssembly);

    if (st >= 0)
    {
        //inspectAssembly("/Users/josh/Desktop/AtomicNETTest.dll");
    }
    */


    /*
    unsigned int exitCode;

    st = sExecuteAssembly(
            hostHandle_,
            domainId_,
            0,
            0,
            "/Users/josh/Desktop/OSX.x64.Debug/HelloWorld.exe",
            (unsigned int*)&exitCode);
    */


#endif

    return true;

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

