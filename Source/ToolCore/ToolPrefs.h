//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

class ToolPrefs : public Object
{
    OBJECT(ToolPrefs)

public:

    ToolPrefs(Context* context);
    virtual ~ToolPrefs();

    const String& GetAndroidSDKPath() { return androidSDKPath_; }
    const String& GetJDKRootPath() { return jdkRootPath_; }
    const String& GetAntPath();

    void SetAndroidSDKPath(const String& path) { androidSDKPath_ = path; }
    void SetJDKRootPath(const String& path) { jdkRootPath_ = path; }
    void SetAntPath(const String& path) { antPath_ = path; }

    String GetPrefsPath();
    void Load();
    void Save();

private:

    String androidSDKPath_;
    String jdkRootPath_;
    String antPath_;

};

}
