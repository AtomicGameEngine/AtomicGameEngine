//
// Copyright (c) 2008-2014 the Urho3D project.
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
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

#include <Atomic/Engine/Engine.h>
#include <Atomic/IO/FileSystem.h>
#include "NETCore.h"
#include <AtomicNET/NETScript/NETScript.h>
#include <AtomicNET/NETScript/CSComponentAssembly.h>

#include "NETIPCServerApp.h"

#ifdef ATOMIC_PLATFORM_OSX
#include <unistd.h>
#endif

#ifdef ATOMIC_PLATFORM_WINDOWS
#include <stdio.h>
#endif

namespace Atomic
{

    NETIPCServerApp::NETIPCServerApp(Context* context) :
        IPCServerApp(context)
    {

    }

    void NETIPCServerApp::Setup()
    {
        IPCServerApp::Setup();

        // TODO: we should always have a --project for IPCPlayer, however it is doing 
        // double duty right now as managed player
        StringVector args = GetArguments();
        if (!args.Contains("--project"))
        {
            engineParameters_["ResourcePrefixPaths"] = "AtomicPlayer_Resources";
            engineParameters_["ResourcePaths"] = "AtomicResources";
        }
    }

    int NETIPCServerApp::Initialize()
    {
        Setup();

        RegisterNETScriptLibrary(context_);

        if (exitCode_)
            return exitCode_;

        if (!engine_->Initialize(engineParameters_))
        {
            ErrorExit();
            return exitCode_;
        }

        // TODO: Proper CSComponent assembly preload
        // For now, user must ensure that assemblies are loaded via
        // referencing their types or load assembly before initializing
        // AtomicNET in application.  This is commented out as it
        // causes an issue with double loading referenced assemblies
        // and the ones in the project file
        // CSComponentAssembly::PreloadClassAssemblies();

        Start();

        if (exitCode_)
            return exitCode_;

        return 0;
    }
    
    NETIPCServerApp* NETIPCServerApp::CreateInternal()
    {
        return new NETIPCServerApp(NETCore::GetContext());
    }

}
