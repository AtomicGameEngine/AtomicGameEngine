
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

    String GetAndroidCommand() const;

    BuildBase* NewBuild(Project* project);

private:

    void PrependAndroidCommandArgs(Vector<String> args);

};

}
