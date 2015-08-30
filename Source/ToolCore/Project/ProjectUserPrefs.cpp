
#include "ProjectUserPrefs.h"

namespace ToolCore
{

ProjectUserPrefs::ProjectUserPrefs(Context* context) : Object(context)
{
#ifdef ATOMIC_PLATFORM_OSX
    defaultPlatform_ = PLATFORMID_MAC;
#else
    defaultPlatform_ = PLATFORMID_WINDOWS;
#endif
}

ProjectUserPrefs::~ProjectUserPrefs()
{

}

}
