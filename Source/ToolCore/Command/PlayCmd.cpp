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

#include "PlayCmd.h"

namespace ToolCore
{

PlayCmd::PlayCmd(Context* context) : Command(context)
{

}

PlayCmd::~PlayCmd()
{

}

bool PlayCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "play")
    {
        errorMsg = "Unable to parse play command";
        return false;
    }

    return true;
}

bool PlayCmd::LaunchPlayerProcess(const String& command, const Vector<String>& args, const String& initialDirectory)
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
        Error(ToString("Unable to launch player process: %s", command.CString()));
        return false;
    }

    handle.wait();

    return true;

}


void PlayCmd::Run()
{
    LOGINFOF("Playing project");

    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    ToolEnvironment* env = GetSubsystem<ToolCore::ToolEnvironment>();
    Project* project = tsystem->GetProject();
    const String& editorBinary = env->GetEditorBinary();

    Vector<String> paths;
    paths.Push(env->GetCoreDataDir());
    paths.Push(env->GetPlayerDataDir());
    paths.Push(project->GetResourcePath());

    // fixme: this is for loading from cache
    paths.Push(project->GetProjectPath());
    paths.Push(project->GetProjectPath() + "Cache");

    String resourcePaths;
    resourcePaths.Join(paths, "!");

    Vector<String> vargs;

    String args = ToString("--player --project \"%s\"", AddTrailingSlash(project->GetProjectPath()).CString());

    vargs = args.Split(' ');
    //vargs.Insert(0, "--player");

    // TODO: use IPC (maybe before this set log location/access the log and output it, we need access to errors)
    LaunchPlayerProcess(editorBinary, vargs, "");

    Finished();

}

}
