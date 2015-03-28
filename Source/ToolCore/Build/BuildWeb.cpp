// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../Project/Project.h"
#include "BuildSystem.h"
#include "BuildWeb.h"

namespace ToolCore
{

BuildWeb::BuildWeb(Context* context, Project* project) : BuildBase(context, project)
{

}

BuildWeb::~BuildWeb()
{

}

void BuildWeb::Initialize()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    String dataPath = tsystem->GetDataPath();
    String projectResources = project->GetResourcePath();
    String coreDataFolder = dataPath + "Atomic/Resources/CoreData/";

    AddResourceDir(coreDataFolder);
    AddResourceDir(projectResources);

    BuildResourceEntries();
}
void BuildWeb::Build(const String& buildPath)
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    Initialize();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

    String dataPath = tsystem->GetDataPath();

    String buildSourceDir  = dataPath + "Atomic/Deployment/Web";

    fileSystem->CreateDir(buildPath_);

    String resourcePackagePath = buildPath_ + "/AtomicResources.data";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildSourceDir + "/AtomicPlayer.html", buildPath_ + "/AtomicPlayer.html");
    fileSystem->Copy(buildSourceDir + "/AtomicPlayer.js", buildPath_ + "/AtomicPlayer.js");

    File file(context_, buildSourceDir + "/AtomicResources_js.template", FILE_READ);
    unsigned size = file.GetSize();

    SharedArrayPtr<char> buffer(new char[size + 1]);
    file.Read(buffer.Get(), size);
    buffer[size] = '\0';

    String resourcejs = (const char*) buffer.Get();

    file.Close();

    file.Open(buildPath_ + "/AtomicResources.data", FILE_READ);
    int rsize = (int) file.GetSize();
    file.Close();

    String request;
    request.AppendWithFormat("new DataRequest(0, %i, 0, 0).open('GET', '/AtomicResources.pak');", rsize);

    resourcejs.Replace("$$ATOMIC_RESOURCES_DATA_REQUEST$$", request);

    file.Open(buildPath_ + "/AtomicResources.js", FILE_WRITE);
    file.Write(resourcejs.CString(), resourcejs.Length());
    file.Close();

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete(PLATFORMID_WEB, buildPath_);

    //fileSystem->SystemCommandAsync("/Applications/Firefox.app/Contents/MacOS/firefox");

}

}
