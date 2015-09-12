//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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

void BuildSystem::BuildComplete(PlatformID platform, const String &buildFolder, bool success, const String& buildMessage)
{
    VariantMap eventData;

    eventData[BuildComplete::P_PLATFORMID] = (unsigned) platform;
    eventData[BuildComplete::P_BUILDFOLDER] = buildFolder;
    eventData[BuildComplete::P_SUCCESS] = success;
    eventData[BuildComplete::P_MESSAGE] = buildMessage;
    SendEvent(E_BUILDCOMPLETE, eventData);

    currentBuild_ = 0;
}



}
