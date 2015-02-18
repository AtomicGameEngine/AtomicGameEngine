// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"

#include <rapidjson/document.h>
#include "rapidjson/prettywriter.h"
#include "rapidjson/filestream.h"

#include <Atomic/Core/Context.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/Log.h>
#include <Atomic/IO/File.h>

#include "AEPreferences.h"

using namespace rapidjson;

namespace AtomicEditor
{

AEPreferences::AEPreferences(Context* context) :
    Object(context)
{
    context->RegisterSubsystem(this);

    Read();
}

AEPreferences::~AEPreferences()
{

}

String AEPreferences::GetPreferencesFullPath()
{
    FileSystem* fs = GetSubsystem<FileSystem>();
    String filepath = fs->GetAppPreferencesDir("AtomicEditor", "Preferences");
    filepath += "prefs.json";
    return filepath;
}

void AEPreferences::Clear()
{
    recentProjects_.Clear();
}

void AEPreferences::Read()
{
    rapidjson::Document document;

    String filepath = GetPreferencesFullPath();

    File jsonFile(context_, filepath);

    if (!jsonFile.IsOpen())
        return;

    String json;
    jsonFile.ReadText(json);

    if (!json.Length())
        return;

    if (document.Parse<0>(json.CString()).HasParseError())
    {
        LOGERRORF("Could not parse JSON data from %s", filepath.CString());
        return;
    }

    Clear();

    const Value::Member* recent_files = document.FindMember("recent_files");
    if (recent_files && recent_files->value.IsArray())
    {
        for (Value::ConstValueIterator itr = recent_files->value.Begin(); itr != recent_files->value.End(); itr++)
        {
             if (!(*itr).IsString())
                 continue;

            String path(itr->GetString());
            recentProjects_.Push(path.CString());
        }
    }

    const Value::Member* android_sdk_path = document.FindMember("android_sdk_path");
    if (android_sdk_path && android_sdk_path->value.IsString())
        androidSDKPath_ = android_sdk_path->value.GetString();

    UpdateRecentFiles(false);

}

void AEPreferences::Write()
{
    String filepath = GetPreferencesFullPath();

    FILE* file = fopen(filepath.CString(), "w");

    if (!file)
        return;

    rapidjson::FileStream s(file);
    rapidjson::PrettyWriter<rapidjson::FileStream> writer(s);

    writer.StartObject();

    // recent files
    writer.String("recent_files");
    writer.StartArray();
    for (unsigned i = 0; i < recentProjects_.Size(); i++)
        writer.String(recentProjects_[i].CString());
    writer.EndArray();

    writer.String("android_sdk_path");
    writer.String(androidSDKPath_.CString());

    writer.EndObject();

    fclose(file);

}

void AEPreferences::UpdateRecentFiles(bool write)
{
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    Vector<String> recentProjects;

    for (unsigned i = 0; i < recentProjects_.Size(); i++)
    {
        String path = recentProjects_[i];

        if (!fileSystem->FileExists(path))
            continue;


        recentProjects.Push(path);

        if (recentProjects.Size() == 10)
            break;
    }

    recentProjects_ = recentProjects;

    if (write)
        Write();
}

void AEPreferences::RegisterRecentProject(const String& fullpath)
{
    if (recentProjects_.Contains(fullpath))
    {
        recentProjects_.Remove(fullpath);
    }

    recentProjects_.Insert(0, fullpath);


    UpdateRecentFiles();

}

}
