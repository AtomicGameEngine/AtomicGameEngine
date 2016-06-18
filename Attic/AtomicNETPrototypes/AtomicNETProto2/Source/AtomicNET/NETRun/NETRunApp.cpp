//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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
