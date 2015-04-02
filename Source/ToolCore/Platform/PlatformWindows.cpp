
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
