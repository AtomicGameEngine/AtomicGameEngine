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

#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>
#include <Atomic/IO/FileSystem.h>

#include <Atomic/Resource/JSONFile.h>

#include "ToolPrefs.h"

namespace ToolCore
{

ToolPrefs::ToolPrefs(Context* context) : Object(context),
    androidSDKPath_(),
    jdkRootPath_(),
    antPath_(),
    releasePath_(),
    releaseCheck_(false)
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
        releasePath_ = androidRoot.Get("releasePath").GetString();
        releaseCheck_ = androidRoot.Get("releaseCheck").GetInt();
    }

}

void ToolPrefs::Save()
{
    String path = GetPrefsPath();
 
    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));
    JSONValue& root = jsonFile->GetRoot();
    root.Clear();
    
    JSONValue androidRoot; 
    androidRoot["androidSDKPath"] = androidSDKPath_;
    androidRoot["jdkRootPath"] = jdkRootPath_;
    androidRoot["antPath"] = antPath_;
    androidRoot["releasePath"] = releasePath_;
    androidRoot["releaseCheck"] = releaseCheck_;
   root["android"] = androidRoot;

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));
    jsonFile->Save(*file, "   ");
    file->Close();

}


}
