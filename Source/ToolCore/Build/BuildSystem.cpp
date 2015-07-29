// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/Core/Context.h>
#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "../Project/ProjectUserPrefs.h"

#include "BuildSystem.h"
#include "BuildEvents.h"

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

    currentBuild_ = queuedBuilds_.Front();
    queuedBuilds_.PopFront();

    currentBuild_->Build(buildPath_);

    return true;
}

void BuildSystem::QueueBuild(BuildBase* buildBase)
{
    queuedBuilds_.Push(SharedPtr<BuildBase>(buildBase));
}

void BuildSystem::BuildComplete(PlatformID platform, const String &buildFolder, bool success, bool fail3D)
{
    VariantMap eventData;

    if (success)
    {
        eventData[BuildComplete::P_PLATFORMID] = (unsigned) platform;
        SendEvent(E_BUILDCOMPLETE, eventData);
        LOGINFOF("Build Success");
    }
    else
    {
        eventData[BuildFailed::P_PLATFORMID] = (unsigned) platform;
        SendEvent(E_BUILDFAILED, eventData);
        LOGINFOF("Build Failed");
    }

    currentBuild_ = 0;
}



}
