
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include <Atomic/Resource/JSONFile.h>

#include "ProjectBuildSettings.h"

namespace ToolCore
{

ProjectBuildSettings::ProjectBuildSettings(Context* context) : Object(context)
{

}

ProjectBuildSettings::~ProjectBuildSettings()
{

}

bool ProjectBuildSettings::Load(const String& path)
{
    return true;
}

void ProjectBuildSettings::Save(const String& path)
{
    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    jsonFile->CreateRoot();

    SharedPtr<File> file(new File(context_, path, FILE_WRITE));

    jsonFile->Save(*file, String("   "));

    file->Close();

}


}
