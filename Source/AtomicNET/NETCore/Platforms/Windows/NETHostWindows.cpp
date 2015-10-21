
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "NETHostWindows.h"

// https://github.com/dotnet/coreclr/blob/master/Documentation/project-docs/clr-configuration-knobs.md
// set COMPLUS_LogEnable=1
// set COMPLUS_LogToConsole=1
// set COMPLUS_LogLevel=9
// set COMPLUS_ManagedLogFacility=0x00001000

namespace Atomic
{

NETHostWindows::NETHostWindows(Context* context) :
    NETHost(context),
    clrRuntimeHost_(0),
    clrModule_(0),
    appDomainID_(0)
{

}

NETHostWindows::~NETHostWindows()
{

}

int NETHostWindows::ExecAssembly(const String& assemblyName, const Vector<String>& args)
{
    return 0;
}

void NETHostWindows::Shutdown()
{

}

bool NETHostWindows::CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut)
{
    if (!clrRuntimeHost_)
        return false;

    HRESULT hr = clrRuntimeHost_->CreateDelegate(appDomainID_, WString(assemblyName).CString(), WString(qualifiedClassName).CString(), WString(methodName).CString(), (INT_PTR *)funcOut);

    if (FAILED(hr))
    {
        return false;
    }

    return true;
}

bool NETHostWindows::Initialize()
{
    // It is very important that this is the native path "\\" vs "/" as find files will return "/" or "\" depending
    // on what you give it, which will result in the domain failing to initialize as coreclr can't handle "/" on init

    if (!coreCLRFilesAbsPath_.Length())
        return false;

    if (!coreCLRTPAPaths_.Length())
        return false;

    if (!coreCLRAssemblyLoadPaths_.Length())
        return false;

    if (!LoadCLRDLL())
        return false;

    if (!InitCLRRuntimeHost())
        return false;

    if (!CreateAppDomain())
        return false;


    return true;
}

void NETHostWindows::WaitForDebuggerConnect()
{
    while (!IsDebuggerPresent())
    {
        Sleep(100);
    }
}

bool NETHostWindows::LoadCLRDLL()
{

    WString wcoreCLRDLLPath(coreCLRFilesAbsPath_ + "coreclr.dll");

    HMODULE result = ::LoadLibraryExW(wcoreCLRDLLPath.CString(), NULL, 0);

    if (!result)
    {
        LOGERRORF("Unable to load CoreCLR from %s", (coreCLRFilesAbsPath_ + "coreclr.dll").CString() );
        return false;
    }

    // Pin the module - CoreCLR.dll does not support being unloaded.
    HMODULE dummy_coreCLRModule;
    if (!::GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_PIN, wcoreCLRDLLPath.CString(), &dummy_coreCLRModule))
    {
        LOGERRORF("Unable to pin CoreCLR module: %s",  (coreCLRFilesAbsPath_ + "coreclr.dll").CString() );
        return false;
    }

    clrModule_ = result;

    return true;
}

bool NETHostWindows::CreateAppDomain()
{
    wchar_t appPath[MAX_LONGPATH] = W("");
    wchar_t appNiPath[MAX_LONGPATH * 2] = W("");

    //wcscpy_s(appPath, WString(coreCLRFilesAbsPath_).CString());

    wcscpy_s(appNiPath, appPath);
    wcscat_s(appNiPath, MAX_LONGPATH * 2, W(";"));
    wcscat_s(appNiPath, MAX_LONGPATH * 2, appPath);

    // Construct native search directory paths
    wchar_t nativeDllSearchDirs[MAX_LONGPATH * 3];

    wcscpy_s(nativeDllSearchDirs, appPath);
    wcscat_s(nativeDllSearchDirs, MAX_LONGPATH * 3, W(";"));
    wcscat_s(nativeDllSearchDirs, MAX_LONGPATH * 3, WString(coreCLRFilesAbsPath_).CString());

    //-------------------------------------------------------------

    // Create an AppDomain

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


    // IMPORTANT: ALL PATHS MUST USE "\" and not "/"

    const wchar_t *property_keys[] = {
        W("TRUSTED_PLATFORM_ASSEMBLIES"),
        W("APP_PATHS"),
        W("APP_NI_PATHS"),
        W("NATIVE_DLL_SEARCH_DIRECTORIES"),
        W("AppDomainCompatSwitch")
    };
    const wchar_t *property_values[] = {
         // TRUSTED_PLATFORM_ASSEMBLIES
         tpaList_.CStr(),
        // APP_PATHS
        appPath,
        // APP_NI_PATHS
        appNiPath,
        // NATIVE_DLL_SEARCH_DIRECTORIES
        nativeDllSearchDirs,
        // AppDomainCompatSwitch
        W("UseLatestBehaviorWhenTFMNotSpecified")
    };

    HRESULT hr  = clrRuntimeHost_->CreateAppDomainWithManager(
                W("AtomicNETDomain"),   // The friendly name of the AppDomain
                // Flags:
                // APPDOMAIN_ENABLE_PLATFORM_SPECIFIC_APPS
                // - By default CoreCLR only allows platform neutral assembly to be run. To allow
                //   assemblies marked as platform specific, include this flag
                //
                // APPDOMAIN_ENABLE_PINVOKE_AND_CLASSIC_COMINTEROP
                // - Allows sandboxed applications to make P/Invoke calls and use COM interop
                //
                // APPDOMAIN_SECURITY_SANDBOXED
                // - Enables sandboxing. If not set, the app is considered full trust
                //
                // APPDOMAIN_IGNORE_UNHANDLED_EXCEPTION
                // - Prevents the application from being torn down if a managed exception is unhandled
                //
                APPDOMAIN_ENABLE_PLATFORM_SPECIFIC_APPS |
                APPDOMAIN_ENABLE_PINVOKE_AND_CLASSIC_COMINTEROP |
                APPDOMAIN_DISABLE_TRANSPARENCY_ENFORCEMENT,
                NULL,                // Name of the assembly that contains the AppDomainManager implementation
                NULL,                    // The AppDomainManager implementation type name
                sizeof(property_keys)/sizeof(wchar_t*),  // The number of properties
                property_keys,
                property_values,
                &appDomainID_);

    if (FAILED(hr)) {
        LOGERRORF("Failed call to CreateAppDomainWithManager. ERRORCODE:%u ",  hr);
        return false;
    }

    return true;
}

bool NETHostWindows::InitCLRRuntimeHost()
{
    if (!clrModule_)
        return false;

    FnGetCLRRuntimeHost pfnGetCLRRuntimeHost =
            (FnGetCLRRuntimeHost)::GetProcAddress(clrModule_, "GetCLRRuntimeHost");

    if (!pfnGetCLRRuntimeHost)
    {
        LOGERRORF("Unable to get GetCLRRuntimeHost function from module: %s",  (coreCLRFilesAbsPath_ + "coreclr.dll").CString() );
        return false;
    }

    HRESULT hr = pfnGetCLRRuntimeHost(IID_ICLRRuntimeHost2, (IUnknown**)&clrRuntimeHost_);

    if (FAILED(hr))
    {
        LOGERRORF("Failed to get ICLRRuntimeHost2 interface. ERRORCODE: %u", hr);
        return false;
    }

    // Set up the startup flags for the clr runtime
        STARTUP_FLAGS dwStartupFlags = (STARTUP_FLAGS)(
            STARTUP_FLAGS::STARTUP_LOADER_OPTIMIZATION_SINGLE_DOMAIN |
            STARTUP_FLAGS::STARTUP_SINGLE_APPDOMAIN /* |
            STARTUP_FLAGS::STARTUP_SERVER_GC*/
            );

    // Default startup flags
    hr = clrRuntimeHost_->SetStartupFlags(dwStartupFlags);

    if (FAILED(hr))
    {
        LOGERRORF("Failed to set startup flags. ERRORCODE: %u", hr);
        return false;
    }

    /*
    // Authenticate with either CORECLR_HOST_AUTHENTICATION_KEY or CORECLR_HOST_AUTHENTICATION_KEY_NONGEN
    hr = clrRuntimeHost_->Authenticate(CORECLR_HOST_AUTHENTICATION_KEY);

    if (FAILED(hr))
    {
        LOGERRORF("CoreCLR failed to authenticate. ERRORCODE: %u", hr);
        return false;
    }
    */


    hr = clrRuntimeHost_->Start();

    if (FAILED(hr))
    {
        LOGERRORF("Failed to start CoreCLR. ERRORCODE: %u", hr);
        return false;
    }

    if (!GenerateTPAList())
    {
        LOGERRORF("Failed to generate TPA List");
        return false;
    }

    return true;
}

bool NETHostWindows::TPAListContainsFile(wchar_t* fileNameWithoutExtension, wchar_t** rgTPAExtensions, int countExtensions)
{
    if (!tpaList_.CStr()) return false;

    for (int iExtension = 0; iExtension < countExtensions; iExtension++)
    {
        wchar_t fileName[MAX_LONGPATH];
        wcscpy_s(fileName, MAX_LONGPATH, W("\\")); // So that we don't match other files that end with the current file name
        wcscat_s(fileName, MAX_LONGPATH, fileNameWithoutExtension);
        wcscat_s(fileName, MAX_LONGPATH, rgTPAExtensions[iExtension] + 1);
        wcscat_s(fileName, MAX_LONGPATH, W(";")); // So that we don't match other files that begin with the current file name

        if (wcsstr(tpaList_.CStr(), fileName))
        {
            return true;
        }
    }
    return false;
}


void NETHostWindows::RemoveExtensionAndNi(wchar_t* fileName)
{
    // Remove extension, if it exists
    wchar_t* extension = wcsrchr(fileName, W('.'));
    if (extension != NULL)
    {
        extension[0] = W('\0');

        // Check for .ni
        size_t len = wcslen(fileName);
        if (len > 3 &&
                fileName[len - 1] == W('i') &&
                fileName[len - 2] == W('n') &&
                fileName[len - 3] == W('.') )
        {
            fileName[len - 3] = W('\0');
        }
    }
}


void NETHostWindows::AddFilesFromDirectoryToTPAList(const wchar_t* targetPath, wchar_t** rgTPAExtensions, int countExtensions)
{

    wchar_t assemblyPath[MAX_LONGPATH];

    for (int iExtension = 0; iExtension < countExtensions; iExtension++)
    {
        wcscpy_s(assemblyPath, MAX_LONGPATH, targetPath);

        const size_t dirLength = wcslen(targetPath);
        wchar_t* const fileNameBuffer = assemblyPath + dirLength;
        const size_t fileNameBufferSize = MAX_LONGPATH - dirLength;

        wcscat_s(assemblyPath, rgTPAExtensions[iExtension]);
        WIN32_FIND_DATA data;
        HANDLE findHandle = FindFirstFile(assemblyPath, &data);

        if (findHandle != INVALID_HANDLE_VALUE)
        {
            do
            {
                if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
                {
                    // It seems that CoreCLR doesn't always use the first instance of an assembly on the TPA list (ni's may be preferred
                    // over il, even if they appear later). So, only include the first instance of a simple assembly name to allow
                    // users the opportunity to override Framework assemblies by placing dlls in %CORE_LIBRARIES%

                    // ToLower for case-insensitive comparisons
                    wchar_t* fileNameChar = data.cFileName;
                    while (*fileNameChar)
                    {
                        *fileNameChar = towlower(*fileNameChar);
                        fileNameChar++;
                    }

                    // Remove extension
                    wchar_t fileNameWithoutExtension[MAX_LONGPATH];
                    wcscpy_s(fileNameWithoutExtension, MAX_LONGPATH, data.cFileName);

                    RemoveExtensionAndNi(fileNameWithoutExtension);

                    // Add to the list if not already on it
                    if (!TPAListContainsFile(fileNameWithoutExtension, rgTPAExtensions, countExtensions))
                    {
                        const size_t fileLength = wcslen(data.cFileName);
                        const size_t assemblyPathLength = dirLength + fileLength;
                        wcsncpy_s(fileNameBuffer, fileNameBufferSize, data.cFileName, fileLength);
                        tpaList_.Append(assemblyPath, assemblyPathLength);
                        tpaList_.Append(W(";"), 1);
                    }
                    else
                    {
                        LOGINFOF("NETHostWindows skipping assembly");
                    }
                }
            } while (0 != FindNextFile(findHandle, &data));

            FindClose(findHandle);
        }
    }
}

bool NETHostWindows::GenerateTPAList()
{

    wchar_t *rgTPAExtensions[] = {
        W("*.ni.dll"),		// Probe for .ni.dll first so that it's preferred if ni and il coexist in the same dir
        W("*.dll"),
        W("*.ni.exe"),
        W("*.exe"),
    };

    AddFilesFromDirectoryToTPAList(WString(coreCLRFilesAbsPath_).CString(), rgTPAExtensions, _countof(rgTPAExtensions));

    Vector<String> paths = coreCLRTPAPaths_.Split(';');
    for (unsigned i = 0; i < paths.Size(); i++)
    {
        AddFilesFromDirectoryToTPAList(WString(GetNativePath(paths[i])).CString(), rgTPAExtensions, _countof(rgTPAExtensions));
    }

    return true;

}


}

/*
RETAIL_CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogEnable, W("LogEnable"), "Turns on the traditional CLR log.")
RETAIL_CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogFacility,  W("LogFacility"),  "Specifies a facility mask for CLR log. (See 'loglf.h'; VM interprets string value as hex number.) Also used by stresslog.")
RETAIL_CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogFacility2, W("LogFacility2"), "Specifies a facility mask for CLR log. (See 'loglf.h'; VM interprets string value as hex number.) Also used by stresslog.")
RETAIL_CONFIG_DWORD_INFO(EXTERNAL_logFatalError, W("logFatalError"), 1, "Specifies whether EventReporter logs fatal errors in the Windows event log.")
CONFIG_STRING_INFO_EX(INTERNAL_LogFile, W("LogFile"), "Specifies a file name for the CLR log.", CLRConfig::REGUTIL_default)
CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogFileAppend, W("LogFileAppend"), "Specifies whether to append to or replace the CLR log file.")
CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogFlushFile, W("LogFlushFile"), "Specifies whether to flush the CLR log file file on each write.")
RETAIL_CONFIG_DWORD_INFO_DIRECT_ACCESS(EXTERNAL_LogLevel, W("LogLevel"), "4=10 msgs, 9=1000000, 10=everything")
RETAIL_CONFIG_STRING_INFO_DIRECT_ACCESS(INTERNAL_LogPath, W("LogPath"), "?Fusion debug log path.")
RETAIL_CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogToConsole, W("LogToConsole"), "Writes the CLR log to console.")
CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogToDebugger, W("LogToDebugger"), "Writes the CLR log to debugger (OutputDebugStringA).")
CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogToFile, W("LogToFile"), "Writes the CLR log to a file.")
CONFIG_DWORD_INFO_DIRECT_ACCESS(INTERNAL_LogWithPid, W("LogWithPid"), "Appends pid to filename for the CLR log.")
RETAIL_CONFIG_DWORD_INFO_EX(EXTERNAL_FusionLogFileNamesIncludePid, W("FusionLogFileNamesIncludePid"), 0, "Fusion logging will append process id to log filenames.", CLRConfig::REGUTIL_default)
*/
