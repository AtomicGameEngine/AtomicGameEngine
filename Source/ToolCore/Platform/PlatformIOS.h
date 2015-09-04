
#pragma once

#include "Platform.h"

namespace ToolCore
{

class PlatformIOS : public Platform
{
    OBJECT(PlatformIOS);

public:

    PlatformIOS(Context* context);
    virtual ~PlatformIOS();

    String GetName() { return "IOS"; }
    PlatformID GetPlatformID() { return PLATFORMID_IOS; }

    BuildBase* NewBuild(Project* project);

    String ParseProvisionAppIdentifierPrefix(const String& provisionFile);

};

}
