

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
