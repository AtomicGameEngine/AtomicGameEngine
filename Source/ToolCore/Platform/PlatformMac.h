
#pragma once

#include "Platform.h"

namespace ToolCore
{

class PlatformMac : public Platform
{
    OBJECT(PlatformMac);

public:

    PlatformMac(Context* context);
    virtual ~PlatformMac();

    String GetName() { return "MAC"; }
    PlatformID GetPlatformID() { return PLATFORMID_MAC; }

    BuildBase* NewBuild(Project* project);

    virtual bool GetLicense() { return true; }

};

}
