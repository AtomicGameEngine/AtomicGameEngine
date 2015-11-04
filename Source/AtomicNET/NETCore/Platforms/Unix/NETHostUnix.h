
#pragma once

#include "../../NETHost.h"

#include <stdio.h>

namespace Atomic
{

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

class ATOMIC_API NETHostUnix : public NETHost
{

    OBJECT(NETHostUnix);

public:
    /// Construct.
    NETHostUnix(Context* context);
    /// Destruct.
    virtual ~NETHostUnix();

    bool Initialize();
    bool CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut);
    void WaitForDebuggerConnect();
    void Shutdown();

    int ExecAssembly(const String& assemblyName, const Vector<String>& args);

private:

    bool LoadCLRDLL(String& errorMsg);
    bool CreateAppDomain(String &errorMsg);
    void AddFilesFromDirectoryToTPAList(const String& targetPath, Vector<String>& trustedAssemblies);
    void GenerateTPAList(String& tpaList);

    InitializeCoreCLRFunction InitializeCoreCLR_;
    ExecuteAssemblyFunction ExecuteAssembly_;
    CreateDelegateFunction CreateDelegate_;
    ShutdownCoreCLRFunction ShutdownCoreCLR_;

    void* coreCLRDLLHandle_;
    void* hostHandle_;
    unsigned domainId_;


};

}
