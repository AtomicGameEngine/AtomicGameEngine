
#pragma once

#include "Platform.h"

namespace ToolCore
{

class PlatformAndroid : public Platform
{
    OBJECT(PlatformAndroid);

public:

    PlatformAndroid(Context* context);
    virtual ~PlatformAndroid();

    String GetName() { return "ANDROID"; }
    PlatformID GetPlatformID() { return PLATFORMID_ANDROID; }

    BuildBase* NewBuild(Project* project);

};

}
