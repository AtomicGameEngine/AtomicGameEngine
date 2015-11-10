//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/Resource/JSONFile.h>

#include "ToolPrefs.h"

namespace ToolCore
{

ToolPrefs::ToolPrefs(Context* context) : Object(context)
{

}

ToolPrefs::~ToolPrefs()
{

}

const String& ToolPrefs::GetAntPath()
{
#ifdef ATOMIC_PLATFORM_OSX
    static String defaultAntPath("/usr/local/bin/ant");
    if (!antPath_.Length())
        return defaultAntPath;
    return antPath_;
#else
    return antPath_;
#endif
}

String ToolPrefs::GetPrefsPath()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    String path = fs->GetAppPreferencesDir("AtomicGameEngine", "ToolCore");
    path += "ToolPrefs.json";
    return path;

}

void ToolPrefs::Load()
{
    String path = GetPrefsPath();

    // Check that the tool prefs file exists
    FileSystem* fs = GetSubsystem<FileSystem>();
    if (!fs->FileExists(path))
        return;

    SharedPtr<File> file(new File(context_, path));

    if (!file->IsOpen())
        return;

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));
    bool result = jsonFile->Load(*file);
    file->Close();

    if (!result)
        return;

    JSONValue root = jsonFile->GetRoot();
    if (!root.IsObject())
        return;

    JSONValue androidRoot = root.Get("android");

    if (androidRoot.IsObject()) {

        androidSDKPath_ = androidRoot.Get("androidSDKPath").GetString();
        jdkRootPath_ = androidRoot.Get("jdkRootPath").GetString();
        antPath_ = androidRoot.Get("antPath").GetString();
    }

}

void ToolPrefs::Save()
{
    String path = GetPrefsPath();

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->GetRoot();

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    JSONValue androidRoot;
    androidRoot.Set("androidSDKPath", androidSDKPath_);
    androidRoot.Set("jdkRootPath", jdkRootPath_);
    androidRoot.Set("antPath", antPath_);
    root.Set("android", androidRoot);

    jsonFile->Save(*file, String("   "));

    file->Close();

}


}
