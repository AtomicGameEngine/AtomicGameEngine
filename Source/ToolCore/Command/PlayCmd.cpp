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
    ATOMIC_LOGINFOF("Playing project");

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
