//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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

    virtual bool GetLicense();

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
