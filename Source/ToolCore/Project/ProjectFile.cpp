//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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

void ProjectFile::WriteNewProject(const String& fullpath)
{
    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->GetRoot();

    root.Set("version", PROJECTFILE_VERSION);

    // project object
    JSONValue jproject;
    jproject.Set("version", "1.0.0");
    root.Set("project", jproject);

    // platforms
    JSONValue platforms(JSONValue::emptyArray);
    root.Set("platforms", platforms);

    SharedPtr<File> file(new File(context_, fullpath, FILE_WRITE));
    jsonFile->Save(*file, String("   "));
    file->Close();

}

void ProjectFile::Save(Project* project)
{
    project_ = project;
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    String fullpath = project->GetProjectFilePath();

    SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

    JSONValue root = jsonFile->GetRoot();

    root.Set("version", PROJECTFILE_VERSION);

    // project object
    JSONValue jproject;
    jproject.Set("version", project_->GetVersion());
    root.Set("project", jproject);

    // platforms
    JSONArray platforms;

    for (List<PlatformID>::ConstIterator i = project_->platforms_.Begin(); i != project_->platforms_.End(); ++i)
    {
        Platform* platform = tsystem->GetPlatformByID(*i);
        if (platform)
        {
            platforms.Push(JSONValue(platform->GetName().ToLower()));
        }
    }

    root.Set("platforms", platforms);

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

    if (file->GetSize() != 0)
    {

        SharedPtr<JSONFile> jsonFile(new JSONFile(context_));

        if (!jsonFile->BeginLoad(*file))
            return false;

        JSONValue root = jsonFile->GetRoot();

        int version = root.Get("version").GetInt();

        if (version != PROJECTFILE_VERSION)
            return false;

        // project object
        JSONValue jproject = root.Get("project");

        if (jproject.IsObject())
        {
            String pversion = jproject.Get("version").GetString();
            project_->SetVersion(pversion);
        }

        JSONValue platforms = root.Get("platforms");
        if (!platforms.IsArray())
            return false;
    }

    // for now, every project gets all platforms

    project_->AddPlatform(PLATFORMID_WINDOWS);
    project_->AddPlatform(PLATFORMID_MAC);
    project_->AddPlatform(PLATFORMID_ANDROID);
    project_->AddPlatform(PLATFORMID_IOS);
    project_->AddPlatform(PLATFORMID_WEB);

    /*
    for (unsigned i = 0; i < platforms.GetSize(); i++)
    {
        String jplatform = platforms.GetString(i);
        Platform* platform = tsystem->GetPlatformByName(jplatform);
        if (platform)
            project_->AddPlatform(platform->GetPlatformID());
    }
    */

    return true;

}


}
