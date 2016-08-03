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
    ATOMIC_OBJECT(ToolEnvironment, Object)

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

    /// Data directories
    const String& GetDeploymentDataDir() { return toolBinary_; }

    const String& GetToolDataDir() { return toolDataDir_; }

    const String& GetDevConfigFilename();

    // AtomicNET

    const String& GetAtomicNETRootDir() { return atomicNETRootDir_; }
    const String& GetAtomicNETCoreAssemblyDir() { return atomicNETCoreAssemblyDir_; }
    const String& GetAtomicNETNuGetBinary() { return atomicNETNuGetBinary_; }
    const String& GetAtomicNETManagedPlayerBinary() { return atomicNETManagedPlayerBinary_; }
    const String& GetAtomicNETManagedIPCPlayerBinary() { return atomicNETManagedIPCPlayerBinary_; }


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

    // AtomicNET
    String atomicNETRootDir_;
    String atomicNETCoreAssemblyDir_;
    String atomicNETNuGetBinary_;
    String atomicNETManagedPlayerBinary_;
    String atomicNETManagedIPCPlayerBinary_;

    SharedPtr<ToolPrefs> toolPrefs_;
};

}
