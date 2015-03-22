
#pragma once

#include <Atomic/Core/Object.h>

#include "Platform/Platform.h"

using namespace Atomic;

namespace ToolCore
{

class Platform;

class ToolSystem : public Object
{
    OBJECT(ToolSystem);

public:

    ToolSystem(Context* context);
    virtual ~ToolSystem();

    void RegisterPlatform(Platform* platform);

    void SetCurrentPlatform(PlatformID platform);
    PlatformID GetCurrentPlatform();

private:

    SharedPtr<Platform> currentPlatform_;

    // PlatformID -> platform
    HashMap<unsigned, SharedPtr<Platform> > platforms_;

};

}
