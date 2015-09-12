//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
    LOGINFOF("Editing project");

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
