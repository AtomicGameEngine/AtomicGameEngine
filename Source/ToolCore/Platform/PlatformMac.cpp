
#include "../Project/Project.h"
#include "../Build/BuildMac.h"
#include "PlatformMac.h"

namespace ToolCore
{

PlatformMac::PlatformMac(Context* context) : Platform(context)
{

}

PlatformMac::~PlatformMac()
{

}

BuildBase* PlatformMac::NewBuild(Project *project)
{
    return new BuildMac(context_, project);
}

}
