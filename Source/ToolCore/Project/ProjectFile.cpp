
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include <Atomic/Resource/JSONFile.h>

#include "../ToolSystem.h"

#include "Project.h"
#include "ProjectFile.h"

namespace ToolCore
{

ProjectFile::ProjectFile(Context* context) : Object(context)
{

}

ProjectFile::~ProjectFile()
{

}

void ProjectFile::Save(Project* project)
{
    project_ = project;
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    String fullpath = project->GetProjectFilePath();

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->CreateRoot();
    root.SetInt("version", PROJECTFILE_VERSION);

    // platforms

    JSONValue platforms = root.CreateChild("platforms", JSON_ARRAY);

    for (List<PlatformID>::ConstIterator i = project_->platforms_.Begin(); i != project_->platforms_.End(); ++i)
    {
        Platform* platform = tsystem->GetPlatformByID(*i);
        if (platform)
        {
            platforms.AddString(platform->GetName().ToLower());
        }
    }

    // Save to file
    SharedPtr<File> file(new File(context_, fullpath, FILE_WRITE));
    jsonFile->Save(*file, String("   "));
    file->Close();

}

bool ProjectFile::Load(Project* project)
{
    project_ = project;
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    String fullpath = project->GetProjectFilePath();

    SharedPtr<File> file(new File(context_, fullpath, FILE_READ));
    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    jsonFile->BeginLoad(*file);

    JSONValue root = jsonFile->GetRoot();

    int version = root.GetInt("version");

    if (version != PROJECTFILE_VERSION)
        return false;

    JSONValue platforms = root.GetChild("platforms");
    if (!platforms.IsArray())
        return false;

    for (unsigned i = 0; i < platforms.GetSize(); i++)
    {
        String jplatform = platforms.GetString(i);
        Platform* platform = tsystem->GetPlatformByName(jplatform);
        if (platform)
            project_->AddPlatform(platform->GetPlatformID());
    }

    return true;

}


}
