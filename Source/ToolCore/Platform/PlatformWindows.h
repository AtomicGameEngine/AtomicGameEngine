
#pragma once

#include "Platform.h"

namespace ToolCore
{

class PlatformWindows : public Platform
{
    OBJECT(PlatformWindows);

public:

    PlatformWindows(Context* context);
    virtual ~PlatformWindows();

    String GetName() { return "WINDOWS"; }
    PlatformID GetPlatformID() { return PLATFORMID_WINDOWS; }

    BuildBase* NewBuild(Project* project);

    virtual bool GetLicense() { return true; }

};

}
