//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"

#include "PlatformAddCmd.h"

#include <Poco/File.h>

namespace ToolCore
{

PlatformAddCmd::PlatformAddCmd(Context* context) : Command(context)
{

}

PlatformAddCmd::~PlatformAddCmd()
{

}

bool PlatformAddCmd::Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg)
{
    String argument = arguments[startIndex].ToLower();
    String value = startIndex + 1 < arguments.Size() ? arguments[startIndex + 1] : String::EMPTY;

    if (argument != "platform-add")
    {
        errorMsg = "Unable to parse build command";
        return false;
    }

    if (!value.Length())
    {
        errorMsg = "Unable to parse platform";
        return false;
    }

    platformToAdd_ = value.ToLower();

    return true;
}

void PlatformAddCmd::Run()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    Platform* platform = tsystem->GetPlatformByName(platformToAdd_);

    if (!platform)
    {
        Error(ToString("Unknown platform: %s", platformToAdd_.CString()));
        return;
    }

    if (project->ContainsPlatform(platform->GetPlatformID()))
    {
        Error(ToString("Project already contains platform: %s", platformToAdd_.CString()));
        return;
    }

    LOGINFOF("Adding platform: %s", platformToAdd_.CString());

    project->AddPlatform(platform->GetPlatformID());

    Finished();
}

}
