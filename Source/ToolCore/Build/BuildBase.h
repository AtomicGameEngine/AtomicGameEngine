//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>
#include "BuildTypes.h"
#include "../Platform/Platform.h"

using namespace Atomic;

namespace ToolCore
{

class ResourcePackager;
class Project;

class BuildBase : public Object
{
    OBJECT(BuildBase);

public:

    BuildBase(Context* context, Project* project, PlatformID platform);
    virtual ~BuildBase();

    virtual void Build(const String& buildPath) = 0;

    // some platforms may want to do their own packaging of resources
    virtual bool UseResourcePackager() { return true; }

    virtual String GetBuildSubfolder() = 0;

    // add in search order, first added is first searched
    // will warn on name conflicts
    void AddResourceDir(const String& dir);

    void BuildLog(const String& message, bool sendEvent = true);
    void BuildWarn(const String& warning, bool sendEvent = true);
    void BuildError(const String& error, bool sendEvent = true);

    /// Fail the current build
    void FailBuild(const String& message);

    /// Converts subprocess output event to a buildoutput event
    void HandleSubprocessOutputEvent(StringHash eventType, VariantMap& eventData);

protected:

    bool BuildClean(const String& path);
    bool BuildRemoveDirectory(const String& path);
    bool BuildCreateDirectory(const String& path);
    bool BuildCopyFile(const String& srcFileName, const String& destFileName);


    void GenerateResourcePackage(const String& resourcePackagePath);

    void BuildResourceEntries();

    void GetDefaultResourcePaths(Vector<String>& paths);
    String GetSettingsDirectory();

    String buildPath_;
    PODVector<BuildResourceEntry*> resourceEntries_;

    bool containsMDL_;
    bool buildFailed_;

private:

    PlatformID platformID_;
    
    Vector<String> buildLog_;
    Vector<String> buildWarnings_;
    Vector<String> buildErrors_;

    void ScanResourceDirectory(const String& resourceDir);

    SharedPtr<Project> project_;
    SharedPtr<ResourcePackager> resourcePackager_;
    Vector<String> resourceDirs_;

};

}
