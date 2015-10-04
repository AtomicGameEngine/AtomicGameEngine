
#include <ThirdParty/SDL/include/SDL.h>

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/Core/StringUtils.h>

#include "CSEventHelper.h"
#include "CSComponent.h"
#include "NETCore.h"
#include "NETManaged.h"
#include "NETCoreThunk.h"

namespace Atomic
{

/*
mcs /nostdlib /noconfig /r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll HelloWorld.cs
*/


#ifdef ATOMIC_PLATFORM_OSX
static const char * const sCoreClrDll = "libcoreclr.dylib";
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

    NetCoreThunkInit();

    assert(!instance_);
    instance_ = this;
    csContext_ = context;

    SharedPtr<NETManaged> managed(new NETManaged(context_));
    context_->RegisterSubsystem(managed);

    SharedPtr<CSEventDispatcher> dispatcher(new CSEventDispatcher(context_));
    context_->RegisterSubsystem(dispatcher);
    context_->AddGlobalEventListener(dispatcher);

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

    tpaList.Join(trustedAssemblies, ":");

    // LOGINFOF("NetCore:: TPALIST - %s", tpaList.CString());

}

void NETCore::Shutdown()
{
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

void csb_Atomic_Test(unsigned id)
{
  LOGINFOF("%u", id);
}

}


bool NETCore::Initialize(const String &coreCLRFilesAbsPath, String& errorMsg)
{
    coreCLRFilesAbsPath_ = AddTrailingSlash(coreCLRFilesAbsPath);

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

    String appPath = "/Users/josh/Desktop/OSX.x64.Debug/";
    Vector<String> nativeSearch;
    nativeSearch.Push(coreCLRFilesAbsPath_);
    nativeSearch.Push("/Users/josh/Dev/atomic/AtomicGameEngineSharp-build/Source/AtomicNET/NETNative");

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
                "",
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


    /*
    void* hm;

    st = sCreateDelegate(hostHandle_,
                    domainId_,
                    "HelloWorld",
                    "Hello1",
                    "CallFromNative",
                    &hm);

    if (st >= 0)
    {
        typedef void (*Hm)(const char* value);
        ((Hm)hm)("Hello From Native");
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
    return true;

}

void RegisterNETCoreLibrary(Context* context)
{
    CSComponent::RegisterObject(context);
}


}

