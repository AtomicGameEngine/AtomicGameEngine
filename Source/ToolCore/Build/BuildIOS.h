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

class BuildIOS : public BuildBase
{
    OBJECT(BuildIOS);

public:

    BuildIOS(Context* context, Project* project);
    virtual ~BuildIOS();

    String GetBuildSubfolder() { return "iOS-Build"; }

    void Build(const String& buildPath);

protected:

    enum BuildPhase
    {
        ConvertPList,
        CodeSign,
        Deploy
    };

    void RunConvertPList();
    void HandleConvertPListComplete(StringHash eventType, VariantMap& eventData);

    void RunCodeSign();
    void HandleCodeSignComplete(StringHash eventType, VariantMap& eventData);

    void RunDeploy();
    void HandleDeployComplete(StringHash eventType, VariantMap& eventData);

    String GenerateInfoPlist();
    String GenerateEntitlements();

    void Initialize();

    BuildPhase currentBuildPhase_;


};

}
