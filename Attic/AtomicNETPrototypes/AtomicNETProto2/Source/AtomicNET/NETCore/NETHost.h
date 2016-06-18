
#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/IO/FileSystem.h>

namespace Atomic
{

class ATOMIC_API NETHost : public Object
{

    OBJECT(NETHost);

public:
    /// Construct.
    NETHost(Context* context);
    /// Destruct.
    virtual ~NETHost();

    // It is important all these paths use native folder delimiter when passed to CoreCLR
    static void SetCoreCLRFilesAbsPath(const String& path) { coreCLRFilesAbsPath_ = GetNativePath(AddTrailingSlash(path)); }
    // ; (semicolon)  delimited list of paths to loads assemblies from
    static void SetCoreCLRAssemblyLoadPaths(const String& path) { coreCLRAssemblyLoadPaths_ = path; }
    // ; (semicolon) delimited list of paths to trusted platform assemblies
    static void SetCoreCLRTPAPaths(const String& path) { coreCLRTPAPaths_ = path; }

    static const String& GetCoreCLRAssemblyLoadPaths() { return coreCLRAssemblyLoadPaths_; }

    virtual bool Initialize() = 0;
    virtual bool CreateDelegate(const String& assemblyName, const String& qualifiedClassName, const String& methodName, void** funcOut) = 0;
    virtual void WaitForDebuggerConnect() = 0;
    virtual int ExecAssembly(const String& assemblyName, const Vector<String>& args) = 0;
    virtual void Shutdown() = 0;

protected:

    static String coreCLRFilesAbsPath_;
    static String coreCLRAssemblyLoadPaths_;
    static String coreCLRTPAPaths_;

};

}
