//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/ProcessUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Engine/Engine.h>
#include <Atomic/Resource/ResourceCache.h>

#include <AtomicNET/NETCore/NETHost.h>
#include <AtomicNET/NETCore/NETCore.h>

#include "NETRunApp.h"

DEFINE_APPLICATION_MAIN(Atomic::NETRunApp)

namespace Atomic
{

NETRunApp::NETRunApp(Context* context) :
    Application(context)
{

}

NETRunApp::~NETRunApp()
{

}

void NETRunApp::Setup()
{

    String coreCLRAbsPath;
    String execAssemblyPath;
    String tpaPaths;

    const Vector<String>& arguments = GetArguments();

    for (unsigned i = 0; i < arguments.Size(); ++i)
    {
        if (arguments[i].Length() > 1)
        {
            String argument = arguments[i].ToLower();
            String value = i + 1 < arguments.Size() ? arguments[i + 1] : String::EMPTY;

            if (argument == "--coreclr-abspath")
            {
                if (!value.Length())
                    ErrorExit("Unable to parse --coreclr-abspath");

                coreCLRAbsPath = AddTrailingSlash(value);
            }
            else if (argument == "--exec-assembly")
            {
                if (!value.Length())
                    ErrorExit("Unable to parse --exec-assembly");

                execAssemblyPath = value;
            }
            else if (argument == "--tpa-paths")
            {
                if (!value.Length())
                    ErrorExit("Unable to parse --tpa-paths");

                tpaPaths = AddTrailingSlash(value);
            }

        }

    }

    if (!coreCLRAbsPath.Length())
        ErrorExit("Unable to parse --coreclr-abspath");

    if (!execAssemblyPath.Length())
        ErrorExit("Unable to parse --exec-assembly");

    if (!tpaPaths.Length())
        ErrorExit("Unable to parse --tpa-paths");

    // Parse assembly
    String pathName, fileName, ext;
    SplitPath(execAssemblyPath, pathName, fileName, ext);

    // this needs to be full path to assembly
    assemblyToExecute_ = execAssemblyPath;

    NETHost::SetCoreCLRFilesAbsPath(coreCLRAbsPath);
    NETHost::SetCoreCLRTPAPaths(tpaPaths);
    NETHost::SetCoreCLRAssemblyLoadPaths(pathName);

    // Instantiate and register the AtomicNET subsystem
    SharedPtr<NETCore> netCore (new NETCore(context_));
    context_->RegisterSubsystem(netCore);
    String netCoreErrorMsg;

    if (!netCore->Initialize(netCoreErrorMsg))
    {
        ErrorExit(ToString("NetCore: Unable to initialize! %s", netCoreErrorMsg.CString()));
    }

    engineParameters_["Headless"] = true;
    engineParameters_["LogLevel"] = LOG_INFO;

    // no default resources (will be initialized later)
    engineParameters_["ResourcePaths"] = "";
}



void NETRunApp::Start()
{
    Application::Start();

    NETCore* netCore = GetSubsystem<NETCore>();

    assemblyArgs_.Push(assemblyToExecute_);
    assemblyArgs_.Push("--help");

    netCore->ExecAssembly(assemblyToExecute_, assemblyArgs_);

    GetSubsystem<Engine>()->Exit();
}

void NETRunApp::Stop()
{
    Application::Stop();

}

void NETRunApp::ErrorExit(const String& message)
{
    engine_->Exit(); // Close the rendering window
    exitCode_ = EXIT_FAILURE;

    // Only for WIN32, otherwise the error messages would be double posted on Mac OS X and Linux platforms
    if (!message.Length())
    {
        #ifdef WIN32
        Atomic::ErrorExit(startupErrors_.Length() ? startupErrors_ :
            "Application has been terminated due to unexpected error.", exitCode_);
        #endif
    }
    else
        Atomic::ErrorExit(message, exitCode_);
}



}


