//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

class FileUtils : public Object
{
    OBJECT(FileUtils);

public:

    /// Construct.
    FileUtils(Context* context);
    /// Destruct.
    ~FileUtils();

    bool CreateDirs(const String& folder);

    String GetMobileProvisionPath();
    String GetAndroidSDKPath(const String& defaultPath);
    String GetAntPath(const String& defaultPath);
    String GetJDKRootPath(const String& defaultPath);
    String OpenProjectFileDialog();
    String NewProjectFileDialog();
    String GetBuildPath(const String& defaultPath);
    void RevealInFinder(const String& fullpath);

private:

};


}
