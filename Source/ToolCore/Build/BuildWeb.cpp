//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"

#include "BuildEvents.h"
#include "BuildSystem.h"
#include "BuildWeb.h"

namespace ToolCore
{

BuildWeb::BuildWeb(Context* context, Project* project) : BuildBase(context, project, PLATFORMID_WEB)
{

}

BuildWeb::~BuildWeb()
{

}

void BuildWeb::Initialize()
{
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();

    Project* project = tsystem->GetProject();

    Vector<String> defaultResourcePaths;
    GetDefaultResourcePaths(defaultResourcePaths);
    String projectResources = project->GetResourcePath();

    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        AddResourceDir(defaultResourcePaths[i]);
    }

    // TODO: smart filtering of cache
    AddResourceDir(project->GetProjectPath() + "Cache/");
    AddResourceDir(projectResources);

    BuildResourceEntries();
}
void BuildWeb::Build(const String& buildPath)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Starting Web Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    Initialize();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (fileSystem->DirExists(buildPath_))
        fileSystem->RemoveDir(buildPath_, true);

    String dataPath = tenv->GetToolDataDir();

    String buildSourceDir  = dataPath + "Deployment/Web";

    fileSystem->CreateDir(buildPath_);

    String resourcePackagePath = buildPath_ + "/AtomicResources.data";
    GenerateResourcePackage(resourcePackagePath);

    fileSystem->Copy(buildSourceDir + "/AtomicPlayer.html", buildPath_ + "/AtomicPlayer.html");
    fileSystem->Copy(buildSourceDir + "/AtomicPlayer.html.mem", buildPath_ + "/AtomicPlayer.html.mem");
    fileSystem->Copy(buildSourceDir + "/AtomicPlayer.js", buildPath_ + "/AtomicPlayer.js");
    fileSystem->Copy(buildSourceDir + "/AtomicLoader.js", buildPath_ + "/AtomicLoader.js");
    fileSystem->Copy(buildSourceDir + "/index.html", buildPath_ + "/index.html");
    fileSystem->Copy(buildSourceDir + "/Atomic_Logo_Header.png", buildPath_ + "/Atomic_Logo_Header.png");

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

    resourcejs.Replace("$$REMOTE_PACKAGE_SIZE$$", ToString("%i", rsize));
    resourcejs.Replace("$$ATOMIC_RESOURCES_DATA_REQUEST$$", request);

    file.Open(buildPath_ + "/AtomicResources.js", FILE_WRITE);
    file.Write(resourcejs.CString(), resourcejs.Length());
    file.Close();

    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Web Deployment Complete</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);


    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete(PLATFORMID_WEB, buildPath_);

    //fileSystem->SystemCommandAsync("/Applications/Firefox.app/Contents/MacOS/firefox");

}

}
