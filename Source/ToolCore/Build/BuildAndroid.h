//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "BuildBase.h"

namespace ToolCore
{

class Project;

class PlatformAndroid;

class BuildAndroid : public BuildBase
{
    OBJECT(BuildAndroid);

public:

    BuildAndroid(Context* context, Project* project);
    virtual ~BuildAndroid();

    void Build(const String& buildPath);

    String GetBuildSubfolder() { return "Android-Build"; }

protected:

    enum BuildPhase
    {
        AndroidUpdateProject,
        AntBuildDebug,
        ADBListDevices,
        ADBInstall,
        ADBStartActivity
    };

    void RunAntDebug();
    void HandleAntDebugComplete(StringHash eventType, VariantMap& eventData);

    void RunADBListDevices();
    void HandleADBListDevicesOutputEvent(StringHash eventType, VariantMap& eventData);
    void HandleADBListDevicesComplete(StringHash eventType, VariantMap& eventData);

    void RunADBInstall();
    void HandleRunADBInstallComplete(StringHash eventType, VariantMap& eventData);

    void RunADBStartActivity();
    void HandleADBStartActivityComplete(StringHash eventType, VariantMap& eventData);

    BuildPhase currentBuildPhase_;

    void Initialize();

    PlatformAndroid* platformAndroid_;
    String deviceListText_;

};

}
