
#pragma once

#include "Platform.h"
#include "../Subprocess/Subprocess.h"

namespace ToolCore
{

EVENT(E_ANDROIDTARGETSREFRESHED, AndroidTargetsRefreshed)
{

}

class PlatformAndroid : public Platform
{
    OBJECT(PlatformAndroid);

    struct AndroidTarget
    {
        unsigned id;
    };

public:

    PlatformAndroid(Context* context);
    virtual ~PlatformAndroid();

    String GetName() { return "ANDROID"; }
    PlatformID GetPlatformID() { return PLATFORMID_ANDROID; }

    String GetAndroidCommand() const;
    String GetADBCommand() const;

    void RefreshAndroidTargets();
    const Vector<String>& GetAndroidTargets() { return androidTargets_; }

    BuildBase* NewBuild(Project* project);

private:

    void HandleRefreshAndroidTargetsEvent(StringHash eventType, VariantMap& eventData);

    SharedPtr<Subprocess> refreshAndroidTargetsProcess_;
    String targetOutput_;

    Vector<String> androidTargets_;

    void PrependAndroidCommandArgs(Vector<String> args);

};

}
