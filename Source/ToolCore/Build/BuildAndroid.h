//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "BuildBase.h"

namespace ToolCore
{

class Project;

class PlatformAndroid;

class BuildAndroid : public BuildBase
{
    ATOMIC_OBJECT(BuildAndroid, BuildBase);

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
