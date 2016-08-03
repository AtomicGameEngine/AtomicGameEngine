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

#include <Poco/Process.h>

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Build/BuildEvents.h"
#include "../Build/BuildSystem.h"

#include "EditCmd.h"

namespace ToolCore
{

EditCmd::EditCmd(Context* context) : Command(context)
{

}

EditCmd::~EditCmd()
{

}

bool EditCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "edit")
    {
        errorMsg = "Unable to parse edit command";
        return false;
    }

    return true;
}

bool EditCmd::LaunchEditorProcess(const String& command, const Vector<String>& args, const String& initialDirectory)
{
    Poco::Process::Args pargs;

    for (unsigned i = 0; i < args.Size(); i++)
        pargs.push_back(args[i].CString());

    std::string pcommand = command.CString();
    std::string pinitialDirectory = initialDirectory.CString();

    // this can take an ENV as well, may come in useful
    Poco::ProcessHandle handle(Poco::Process::launch(pcommand, pargs, pinitialDirectory));

    if (!Poco::Process::isRunning(handle))
    {
        Error(ToString("Unable to launch editor process: %s", command.CString()));
        return false;
    }

    handle.wait();

    return true;

}


void EditCmd::Run()
{
    ATOMIC_LOGINFOF("Editing project");

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    ToolEnvironment* env = GetSubsystem<ToolCore::ToolEnvironment>();
    Project* project = tsystem->GetProject();
    const String& editorBinary = env->GetEditorBinary();

    Vector<String> vargs;
    vargs.Push("--project");
    vargs.Push(project->GetProjectPath());

    LaunchEditorProcess(editorBinary, vargs, "");

    Finished();

}

}
