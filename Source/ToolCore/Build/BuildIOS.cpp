// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/IO/FileSystem.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "BuildIOS.h"
#include "BuildSystem.h"

namespace ToolCore
{

BuildIOS::BuildIOS(Context * context, Project *project) : BuildBase(context, project)
{

}

BuildIOS::~BuildIOS()
{

}

void BuildIOS::Initialize()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    Vector<String> defaultResourcePaths;
    GetDefaultResourcePaths(defaultResourcePaths);
    String projectResources = project->GetResourcePath();

    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        AddResourceDir(defaultResourcePaths[i]);
    }

    // TODO: smart filtering of cache
    AddResourceDir(project->GetProjectPath() + "Cache/");
    AddResourceDir(projectResources);

    BuildResourceEntries();

}

void BuildIOS::Build(const String& buildPath)
{

}

}
