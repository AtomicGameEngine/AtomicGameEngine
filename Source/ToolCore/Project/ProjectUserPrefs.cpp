
#include "ProjectUserPrefs.h"

#include <Atomic/IO/File.h>
#include <Atomic/Resource/JSONFile.h>

namespace ToolCore
{

ProjectUserPrefs::ProjectUserPrefs(Context* context) : Object(context)
{
#ifdef ATOMIC_PLATFORM_OSX
    defaultPlatform_ = PLATFORMID_MAC;
#else
    defaultPlatform_ = PLATFORMID_WINDOWS;
#endif
}

ProjectUserPrefs::~ProjectUserPrefs()
{

}

bool ProjectUserPrefs::Load(const String& path)
{
    SharedPtr<File> file(new File(context_, path));
    if (!file->IsOpen())
        return false;

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));
    bool result = jsonFile->Load(*file);
    file->Close();

    if (!result)
        return false;

    JSONValue root = jsonFile->GetRoot();
    if (!root.IsObject())
        return false;

    lastBuildPath_ = root.Get("lastBuildPath").GetString();

    return true;
}

void ProjectUserPrefs::Save(const String& path)
{

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->GetRoot();

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    root.Set("lastBuildPath", JSONValue(lastBuildPath_));

    jsonFile->Save(*file, String("   "));

    file->Close();


}


}
