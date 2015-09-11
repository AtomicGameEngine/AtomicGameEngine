//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//


#include "../Build/BuildWeb.h"
#include "PlatformWeb.h"

namespace ToolCore
{

PlatformWeb::PlatformWeb(Context* context) : Platform(context)
{

}

PlatformWeb::~PlatformWeb()
{

}

BuildBase* PlatformWeb::NewBuild(Project *project)
{
    return new BuildWeb(context_, project);
}


}
