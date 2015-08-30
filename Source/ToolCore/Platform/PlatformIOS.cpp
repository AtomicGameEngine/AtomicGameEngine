

#include "../Build/BuildWeb.h"
#include "PlatformIOS.h"

namespace ToolCore
{

PlatformIOS::PlatformIOS(Context* context) : Platform(context)
{

}

PlatformIOS::~PlatformIOS()
{

}

BuildBase* PlatformIOS::NewBuild(Project *project)
{
    return 0;
}


}
