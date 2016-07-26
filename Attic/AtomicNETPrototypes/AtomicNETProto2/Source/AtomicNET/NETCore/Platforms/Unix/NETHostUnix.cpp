
#include <ThirdParty/SDL/include/SDL.h>

#include <Atomic/IO/Log.h>
#include <Atomic/Core/StringUtils.h>
#include "NETHostUnix.h"

/*
mcs /nostdlib /noconfig /r:System.Console.dll /r:System.Runtime.dll /r:System.IO.dll /r:System.IO.FileSystem.dll /r:mscorlib.dll HelloWorld.cs

export PAL_DBG_CHANNELS="+all.all"
export PAL_DBG_CHANNELS="+PAL.TRACE:+LOADER.TRACE:+ARCH.TRACE:+EXCEPT.ENTRY:+DEBUG.TRACE:+LOCALE.TRACE:+MISC.WARN:+HANDLE.TRACE:+UNICODE.TRACE:-FILE.ERROR"

// see bottom of file for channels

*/


#ifdef ATOMIC_PLATFORM_OSX
static const char * const sCoreClrDll = "libcoreclr.dylib";
#elif ATOMIC_PLATFORM_WINDOWS
static const char * const sCoreClrDll = "coreclr.dll";
#else
static const char * const sCoreClrDll = "libcoreclr.so";
#endif

namespace Atomic
{

/// Construct.
NETHostUnix::NETHostUnix(Context* context) : NETHost(context),
   InitializeCoreCLR_(0),
   ExecuteAssembly_(0),
   CreateDelegate_(0),
   ShutdownCoreCLR_(0),
   coreCLRDLLHandle_(0),
   hostHandle_(0),
   domainId_(0)
{

}

/// Destruct.
NETHostUnix::~NETHostUnix()
{

}

bool NETHostUnix::Initialize()
{
    if (!coreCLRFilesAbsPath_.Length())
        return false;

    if (!coreCLRTPAPaths_.Length())
        return false;

    if (!coreCLRAssemblyLoadPaths_.Length())
        return false;

    String errorMsg;
    if (!LoadCLRDLL(errorMsg))
        return false;

    if (!CreateAppDomain(errorMsg))
        return false;

    return true;
}

int NETHostUnix::ExecAssembly(const String& assemblyName, const Vector<String>& args)
{
    //void* hostHandle,
    //unsigned int domainId,
    //int argc,
    //const char** argv,
    //const char* managedAssemblyPath,
    //unsigned int* exitCode);

    const char** _argv { 0 };

    PODVector<const char*> argv;
    for (unsigned i = 0; i < args.Size(); i++)
        argv.Push(args[i].CString());

    unsigned int exitCode = 0;

    ExecuteAssembly_(hostHandle_, domainId_, (int) argv.Size(), argv.Size() ? &argv[0] : _argv, assemblyName.CString(), &exitCode);

    return (int) exitCode;
}

bool NETHostUnix::CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut)
{

    if (!CreateDelegate_)
        return false;

    *funcOut = 0;

    // TODO: wrap in SharedPtr to control delegate lifetime?
    int st = CreateDelegate_(hostHandle_,
                    domainId_,
                    assemblyName.CString(),
                    qualifiedClassName.CString(),
                    methodName.CString(),
                    funcOut);

    // ensure ptr isn't uninitialized
    if (st < 0)
        *funcOut = 0;

    return st >= 0;

}

void NETHostUnix::WaitForDebuggerConnect()
{

}

void NETHostUnix::AddFilesFromDirectoryToTPAList(const String& targetPath, Vector<String>& trustedAssemblies)
{
    String _targetPath = AddTrailingSlash(targetPath);

    FileSystem* fs = GetSubsystem<FileSystem>();
    Vector<String> results;
    fs->ScanDir(results, _targetPath, "*.dll", SCAN_FILES, true);

    for (unsigned i = 0; i < results.Size(); i++)
    {
        const String& assembly = results[i];

        // TODO: apply filtering to catch duplicates/handle native images, etc
        trustedAssemblies.Push(_targetPath + assembly);
    }


}

void NETHostUnix::GenerateTPAList(String& tpaList)
{
    tpaList = String::EMPTY;

    Vector<String> trustedAssemblies;

    // Add the CoreCLR assemblies to TPA
    AddFilesFromDirectoryToTPAList(coreCLRFilesAbsPath_, trustedAssemblies);

    Vector<String> paths = coreCLRTPAPaths_.Split(';');
    for (unsigned i = 0; i < paths.Size(); i++)
    {
        AddFilesFromDirectoryToTPAList(paths[i], trustedAssemblies);
    }

    /*
    for (unsigned i = 0; i < trustedAssemblies.Size(); i++)
    {
        LOGINFOF("TPA: %s", trustedAssemblies[i].CString());
    }
    */

    // Note : instead of ; here
    tpaList.Join(trustedAssemblies, ":");

}


bool NETHostUnix::CreateAppDomain(String& errorMsg)
{
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

    String appPath = "";
    Vector<String> nativeSearch;
    nativeSearch.Push(coreCLRFilesAbsPath_);
    nativeSearch.Push("");

    String nativeDllSearchDirs;
    nativeDllSearchDirs.Join(nativeSearch, ";");

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

    int st = InitializeCoreCLR_(
                "AtomicEditor",
                "AtomicNETDomain",
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

    return true;
}

bool NETHostUnix::LoadCLRDLL(String& errorMsg)
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

    InitializeCoreCLR_ = (InitializeCoreCLRFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_initialize");

    if (!InitializeCoreCLR_)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_initialize entry point in %s", coreClrDllPath.CString());
        return false;
    }

    ShutdownCoreCLR_ = (ShutdownCoreCLRFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_shutdown");

    if (!ShutdownCoreCLR_)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_shutdown entry point in %s", coreClrDllPath.CString());
        return false;
    }

    CreateDelegate_ = (CreateDelegateFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_create_delegate");

    if (!CreateDelegate_)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_create_delegate entry point in %s", coreClrDllPath.CString());
        return false;
    }

    ExecuteAssembly_ = (ExecuteAssemblyFunction) SDL_LoadFunction(coreCLRDLLHandle_, "coreclr_execute_assembly");

    if (!ExecuteAssembly_)
    {
        errorMsg = ToString("NETCore: Unable to get coreclr_execute_assembly entry point in %s", coreClrDllPath.CString());
        return false;
    }

    return true;
}

void NETHostUnix::Shutdown()
{
    if (ShutdownCoreCLR_ && hostHandle_)
    {
        ShutdownCoreCLR_(hostHandle_, domainId_);
    }

    if (coreCLRDLLHandle_)
    {
        SDL_UnloadObject(coreCLRDLLHandle_);
    }

    coreCLRDLLHandle_ = 0;
    hostHandle_ = 0;
    domainId_ = 0;
    InitializeCoreCLR_ = 0;
    ExecuteAssembly_ = 0;
    CreateDelegate_ = 0;
    ShutdownCoreCLR_ = 0;

}



}

/*
static const char *dbg_channel_names[]=
{
    "PAL",
    "LOADER",
    "HANDLE",
    "SHMEM",
    "THREAD",
    "EXCEPT",
    "CRT",
    "UNICODE",
    "ARCH",
    "SYNC",
    "FILE",
    "VIRTUAL",
    "MEM",
    "SOCKET",
    "DEBUG",
    "LOCALE",
    "MISC",
    "MUTEX",
    "CRITSEC",
    "POLL",
    "CRYPT",
    "SHFOLDER"
#ifdef FEATURE_PAL_SXS
  , "SXS"
#endif // FEATURE_PAL_SXS
};

static const char *dbg_level_names[]=
{
    "ENTRY",
    "TRACE",
    "WARN",
    "ERROR",
    "ASSERT",
    "EXIT"
};
*/
