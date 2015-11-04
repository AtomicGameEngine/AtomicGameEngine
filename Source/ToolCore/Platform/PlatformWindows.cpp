//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include "../Project/Project.h"
#include "../Build/BuildWindows.h"
#include "PlatformWindows.h"

namespace ToolCore
{

PlatformWindows::PlatformWindows(Context* context) : Platform(context)
{

}

PlatformWindows::~PlatformWindows()
{

}

BuildBase* PlatformWindows::NewBuild(Project *project)
{
    return new BuildWindows(context_, project);
}

}
