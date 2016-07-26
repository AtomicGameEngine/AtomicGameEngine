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

#include <Atomic/Core/StringUtils.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/IO/File.h>
#include <Atomic/Resource/ResourceCache.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Assets/AssetDatabase.h"

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

    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        AddResourceDir(defaultResourcePaths[i]);
    }
    BuildDefaultResourceEntries();

    // TODO: smart filtering of cache
    String projectResources = project->GetResourcePath();
    AddProjectResourceDir(projectResources);
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();
    AddProjectResourceDir(cachePath);

    BuildProjectResourceEntries();
}

void BuildWeb::Build(const String& buildPath)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    VariantMap buildOutput;
    buildOutput[BuildOutput::P_TEXT] = "\n\n<color #D4FB79>Starting Web Deployment</color>\n\n";
    SendEvent(E_BUILDOUTPUT, buildOutput);

    Initialize();

    if (!BuildClean(buildPath_))
        return;

    String dataPath = tenv->GetToolDataDir();

    String buildSourceDir  = dataPath + "Deployment/Web";

    if (!BuildCreateDirectory(buildPath_))
        return;

    String resourcePackagePath = buildPath_ + "/AtomicResources.data";
    GenerateResourcePackage(resourcePackagePath);

    if (!BuildCopyFile(buildSourceDir + "/AtomicPlayer.html", buildPath_ + "/AtomicPlayer.html"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/AtomicPlayer.html.mem", buildPath_ + "/AtomicPlayer.html.mem"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/AtomicPlayer.js", buildPath_ + "/AtomicPlayer.js"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/AtomicLoader.js", buildPath_ + "/AtomicLoader.js"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/index.html", buildPath_ + "/index.html"))
        return;
    if (!BuildCopyFile(buildSourceDir + "/Atomic_Logo_Header.png", buildPath_ + "/Atomic_Logo_Header.png"))
        return;

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
    request.AppendWithFormat("new DataRequest(0, %i, 0, 0).open('GET', '/AtomicResources%s');", rsize, PAK_EXTENSION);

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
