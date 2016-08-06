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

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/IPC/IPC.h>
#include <Atomic/IPC/IPCEvents.h>
#include <Atomic/IPC/IPCBroker.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"

#include "NETCmd.h"
#include "../NETTools/AtomicNETService.h"
#include "../NETTools/NETBuildSystem.h"
#include "../NETTools/NETProjectGen.h"

namespace ToolCore
{

NETCmd::NETCmd(Context* context) : Command(context)
{

}

NETCmd::~NETCmd()
{

}

bool NETCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    command_ = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1].ToLower() : String::EMPTY;

    if (argument != "net")
    {
        errorMsg = "Unable to parse net command";
        return false;
    }

    if (command_ == "genproject")
    {
        projectFile_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;
        scriptPlatform_ = startIndex + 3 < arguments.Size() ? arguments[startIndex + 3] : String::EMPTY;

        if (!projectFile_.Length())
        {
            errorMsg = "Unable to parse project file";
            return false;
        }


        if (!scriptPlatform_.Length())
        {
            errorMsg = "Unable to parse script platform";
            return false;
        }
            
    }
    else if (command_ == "parse")
    {
        assemblyPath_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;

        bool exists = false;

        if (assemblyPath_.Length())
        {
            FileSystem* fs = GetSubsystem<FileSystem>();
            exists = fs->FileExists(assemblyPath_);
        }

        if (!exists)
        {
            errorMsg = ToString("Assembly file not found: %s", assemblyPath_.CString());
            return false;
        }

        return true;
    }
    else if (command_ == "compile")
    {
        solutionPath_ = startIndex + 2 < arguments.Size() ? arguments[startIndex + 2] : String::EMPTY;
        platform_ = startIndex + 3 < arguments.Size() ? arguments[startIndex + 3] : String::EMPTY;
        configuration_ = startIndex + 4 < arguments.Size() ? arguments[startIndex + 4] : "Release";

        bool exists = false;

        if (solutionPath_.Length())
        {
            FileSystem* fs = GetSubsystem<FileSystem>();
            exists = fs->FileExists(solutionPath_);
        }

        if (!exists)
        {
            errorMsg = ToString("Solution file not found: %s", solutionPath_.CString());
            return false;
        }

        if (!platform_.Length())
        {
            errorMsg = "Platform not specified";
            return false;
        }

        return true;
    }
    else
    {
        errorMsg = "Unknown net command";
        return false;
    }


    return true;
}

void NETCmd::HandleNETBuildResult(StringHash eventType, VariantMap& eventData)
{
    using namespace NETBuildResult;

    if (eventData[P_SUCCESS].GetBool())
    {
        ATOMIC_LOGINFOF("NETBuild Success for solution: %s", solutionPath_.CString());
        Finished();
    }
    else
    {
        const String& errorText = eventData[P_ERRORTEXT].GetString();

        ATOMIC_LOGERRORF("\n%s\n", errorText.CString());
        Error(ToString("NETBuild Error for solution: %s", solutionPath_.CString()));
        Finished();
    }
    
}

void NETCmd::Run()
{
    if (command_ == "parse")
    {
        // start the NETService, which means we also need IPC
        IPC* ipc = new IPC(context_);
        context_->RegisterSubsystem(ipc);

        netService_ = new AtomicNETService(context_);
        context_->RegisterSubsystem(netService_);

        /*

        VariantMap cmd;
        cmd["command"] = "parse";
        cmd["assemblyPath"] = assemblyPath_;

        netService_->QueueCommand(cmd);

        cmd.Clear();
        cmd["command"] = "exit";

        netService_->QueueCommand(cmd);

        */

        if (!netService_->Start())
        {
            Error("Unable to start AtomicNETService");
            Finished();
        }
    }
    else if (command_ == "compile")
    {

        NETBuildSystem* buildSystem = new NETBuildSystem(context_);
        context_->RegisterSubsystem(buildSystem);
       
        NETBuild* build = buildSystem->Build(solutionPath_, platform_, configuration_);

        if (!build)
        {
            Error("Unable to start build");
            Finished();
            return;
        }

        build->SubscribeToEvent(E_NETBUILDRESULT, ATOMIC_HANDLER(NETCmd, HandleNETBuildResult));

    }
    else if (command_ == "genproject")
    {
        SharedPtr<NETProjectGen> gen(new NETProjectGen(context_));

        gen->SetScriptPlatform(scriptPlatform_);
        gen->LoadProject(projectFile_);

        gen->Generate();

        Finished();

    }

}

}
