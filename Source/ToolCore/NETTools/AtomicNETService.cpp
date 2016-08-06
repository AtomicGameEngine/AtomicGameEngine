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

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include <Atomic/Core/CoreEvents.h>

#include <ToolCore/ToolEnvironment.h>
#include <ToolCore/ToolSystem.h>

#include "../Command/NETCmd.h"

#include "AtomicNETService.h"

namespace ToolCore
{

    AtomicNETService::AtomicNETService(Context* context) :
        IPCServer(context)
    {

    }

    AtomicNETService::~AtomicNETService()
    {

    }

    bool AtomicNETService::GetServiceExecutable(String& execPath, Vector<String>& args) const
    {
        // TODO: Needs to handle deployed tooling, release builds

        execPath = String::EMPTY;
        args.Clear();

#ifdef ATOMIC_PLATFORM_WINDOWS

        ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

#ifdef _DEBUG

        String config = "Debug";
#else
        String config = "Release";
#endif

        execPath = tenv->GetAtomicNETCoreAssemblyDir() + "/AtomicNETService.exe";

#elif ATOMIC_PLATFORM_OSX

        execPath = "/usr/local/share/dotnet/dotnet";

        args.Push("exec");
        args.Push("--additionalprobingpath");
        args.Push("/Users/josh/.nuget/packages");
        args.Push("/Users/josh/Dev/atomic/AtomicGameEngine/Script/AtomicNET/AtomicNETService/bin/Debug/netcoreapp1.0/AtomicNETService.exe");

#endif

        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        if (!fileSystem->FileExists(execPath))
        {
            ATOMIC_LOGERRORF("AtomicNETService binary not found: %s", execPath.CString());
            return false;
        }

        return true;
    }

    bool AtomicNETService::Start()
    {
        String exec;
        Vector<String> args;

        if (!GetServiceExecutable(exec, args))
            return false;

        if (!IPCServer::StartInternal(exec, args))
        {
            return false;
        }

        return true;

    }

}
