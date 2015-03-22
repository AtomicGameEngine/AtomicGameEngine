
#pragma once

#include "Platform.h"

namespace ToolCore
{

class PlatformWeb : public Platform
{
    OBJECT(PlatformWeb);

public:

    PlatformWeb(Context* context);
    virtual ~PlatformWeb();

    String GetName() { return "WEB"; }
    PlatformID GetPlatformID() { return PLATFORMID_WEB; }

};

}
