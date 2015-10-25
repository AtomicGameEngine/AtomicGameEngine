//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

#include "ToolPrefs.h"

using namespace Atomic;

namespace ToolCore
{

// the tool environment contains paths for various
// binaries, data paths, example folder, etc

// it is either built from the cli, AtomicEditor, environment variables,
// or a json config file, this avoids needing to symlink folders, etc

class ToolEnvironment : public Object
{
    OBJECT(ToolEnvironment)

public:

    ToolEnvironment(Context* context);
    virtual ~ToolEnvironment();

    bool InitFromPackage();

    // dev build init env from json
    bool InitFromJSON(bool atomicTool = false);

    /// Root source and build directories for development source tree builds
    void SetRootSourceDir(const String& sourceDir);
    void SetRootBuildDir(const String& buildDir, bool setBinaryPaths = false);

    ToolPrefs* GetToolPrefs() { return toolPrefs_; }
    void SaveToolPrefs() { toolPrefs_->Save(); }
    void LoadToolPrefs() { toolPrefs_->Load(); }

    const String& GetRootSourceDir() { return rootSourceDir_; }
    const String& GetRootBuildDir() { return rootBuildDir_; }

    /// Binaries
    const String& GetEditorBinary() { return editorBinary_; }
    const String& GetPlayerBinary() { return playerBinary_; }
    const String& GetToolBinary() { return toolBinary_; }

    /// Resource directories
    const String& GetCoreDataDir() { return resourceCoreDataDir_; }
    const String& GetPlayerDataDir() { return resourcePlayerDataDir_; }
    const String& GetEditorDataDir() { return resourceEditorDataDir_; }

    /// AtomicNET
    const String& GetNETCoreCLRAbsPath() { return netCoreCLRAbsPath_; }
    const String& GetNETAssemblyLoadPaths() { return netAssemblyLoadPaths_; }
    const String& GetNETTPAPaths() { return netTPAPaths_; }
    const String& GetAtomicNETEngineAssemblyPath() { return netAtomicNETEngineAssemblyPath_; }

    /// Data directories
    const String& GetDeploymentDataDir() { return toolBinary_; }

    const String& GetToolDataDir() { return toolDataDir_; }

    const String& GetDevConfigFilename();

    // OSX
    const String& GetPlayerAppFolder() { return playerAppFolder_; }

    // iOS
    String GetIOSDeployBinary();

    void Dump();

private:

    // root source directory (for development builds)
    String rootSourceDir_;

    // root build directory (for development builds)
    String rootBuildDir_;

    // path to the Atomic Editor binary
    String editorBinary_;

    // path to Atomic player binary used when running content from the editor or cli
    String playerBinary_;

    // path to Atomic player app (OSX)
    String playerAppFolder_;

    // path to the AtomicTool command line binary
    String toolBinary_;

    String toolDataDir_;

    // AtomicNET
    String netCoreCLRAbsPath_;
    String netAssemblyLoadPaths_;
    String netTPAPaths_;
    String netAtomicNETEngineAssemblyPath_;

    // resources
    String resourceCoreDataDir_;
    String resourcePlayerDataDir_;
    String resourceEditorDataDir_;

    // deployment

    // static deployment data directory
    String deploymentDataDir_;

    // whether to use individual build folders, or the deployment data dir for binaries
    bool useBuildDirs_;

    String macBuildDir_;
    String windowsBuildDir_;
    String linuxBuildDir_;

    String androidBuildDir_;
    String iosBuildDir_;
    String webBuildDir_;

    String devConfigFilename_;

    SharedPtr<ToolPrefs> toolPrefs_;
};

}
