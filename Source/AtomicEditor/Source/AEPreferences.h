// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>
#include <Atomic/IO/FileSystem.h>

using namespace Atomic;

namespace AtomicEditor
{

class AEPreferences : public Object
{

    OBJECT(AEPreferences);

public:

    struct StartupPreferences
    {
        IntVector2 windowPos;
        int windowWidth;
        int windowHeight;
    };

    /// Construct.
    AEPreferences(Context* context);
    /// Destruct.
    ~AEPreferences();

    void RegisterRecentProject(const String& fullpath);
    const Vector<String>& GetRecentProjects() { return recentProjects_; }

    void SetAndroidSDKPath(const String& path) { androidSDKPath_ = path; Write(); }
    String GetAndroidSDKPath() { return AddTrailingSlash(androidSDKPath_); }

    void Read();
    void Write();

    static bool ReadStartupPrefs(Context* context, StartupPreferences& prefs);

    void UpdateRecentFiles(bool write = true);

private:

    void Clear();

    String GetPreferencesFullPath();

    String androidSDKPath_;

    String lastProjectFullPath_;
    Vector<String> recentProjects_;    
};


}
