

#include "../Build/BuildWeb.h"
#include "PlatformAndroid.h"

namespace ToolCore
{

PlatformAndroid::PlatformAndroid(Context* context) : Platform(context)
{

}

PlatformAndroid::~PlatformAndroid()
{

}

BuildBase* PlatformAndroid::NewBuild(Project *project)
{
    return 0;
}


}
