// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "../Project/ProjectUserPrefs.h"

#include "BuildSystem.h"
#include "BuildWeb.h"


namespace ToolCore
{

BuildSystem::BuildSystem(Context* context) :
    Object(context)
{

}

BuildSystem::~BuildSystem()
{

}

bool BuildSystem::StartNextBuild()
{
    if (!queuedBuilds_.Size())
        return false;

    SharedPtr<BuildBase> build = queuedBuilds_.Front();
    queuedBuilds_.PopFront();

    build->Build(buildPath_);

    return true;
}

void BuildSystem::QueueBuild(BuildBase* buildBase)
{
    queuedBuilds_.Push(SharedPtr<BuildBase>(buildBase));
}

void BuildSystem::BuildComplete(PlatformID platform, const String &buildFolder, bool success, bool fail3D)
{

    if (success)
        LOGINFOF("Build Success");
    else
        LOGINFOF("Build Failed");

    currentBuild_ = 0;
}



}
