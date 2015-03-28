
#include "ProjectUserPrefs.h"

namespace ToolCore
{

ProjectUserPrefs::ProjectUserPrefs(Context* context) : Object(context)
{
    defaultPlatform_ = PLATFORMID_UNDEFINED;
}

ProjectUserPrefs::~ProjectUserPrefs()
{

}

}
