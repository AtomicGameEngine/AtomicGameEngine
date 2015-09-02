
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

    SharedPtr<File> file(new File(context_, path));

    if (!file->IsOpen())
        return;

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));
    bool result = jsonFile->Load(*file);
    file->Close();

    if (!result)
        return;

    JSONValue root = jsonFile->GetRoot();
    if (root == JSONValue::EMPTY)
        return;

    JSONValue androidRoot = root.GetChild("android");

    if (androidRoot.IsObject()) {

        androidSDKPath_ = androidRoot.GetString("androidSDKPath");
        jdkRootPath_ = androidRoot.GetString("jdkRootPath");
        antPath_ = androidRoot.GetString("antPath");
    }

}

void ToolPrefs::Save()
{
    String path = GetPrefsPath();

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->CreateRoot();

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    JSONValue androidRoot = root.CreateChild("android");
    androidRoot.SetString("androidSDKPath", androidSDKPath_);
    androidRoot.SetString("jdkRootPath", jdkRootPath_);
    androidRoot.SetString("antPath", antPath_);

    jsonFile->Save(*file, String("   "));

    file->Close();

}


}
