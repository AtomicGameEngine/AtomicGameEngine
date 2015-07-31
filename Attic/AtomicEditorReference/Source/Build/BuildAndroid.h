// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include "BuildBase.h"

namespace AtomicEditor
{

class BuildAndroid : public BuildBase
{
    OBJECT(BuildAndroid);

public:

    BuildAndroid(Context* context);
    virtual ~BuildAndroid();

    void Build(const String& buildPath);

    virtual bool UseResourcePackager() { return false; }

protected:

    void Initialize();

private:

    void HandleEvent(StringHash eventType, VariantMap& eventData);

    // subprocesses
    void RunAndroidUpdate();
    void RunAntDebug();
    void RunADBInstall();
    void RunADBListDevices();
    void RunStartActivity();

    enum BuildPhase
    {        
        AndroidUpdateProject,
        AntBuildDebug,        
        ADBListDevices,
        ADBInstall,
        ADBStartActivity
    };

    BuildPhase currentBuildPhase_;
    String deviceListText_;
    String androidSDKPath_;
    String androidCommand_;
    String adbCommand_;
    String antCommand_;

};

}
