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
#include <Atomic/Resource/ResourceCache.h>

#include "../ToolSystem.h"
#include "../ToolEnvironment.h"
#include "../Project/Project.h"
#include "../Assets/AssetDatabase.h"

#include "BuildWindows.h"
#include "BuildSystem.h"
#include "BuildEvents.h"

namespace ToolCore
{

BuildWindows::BuildWindows(Context * context, Project *project) : BuildBase(context, project, PLATFORMID_WINDOWS)
{

}

BuildWindows::~BuildWindows()
{

}

void BuildWindows::Initialize()
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

bool BuildWindows::CheckIncludeResourceFile(const String& resourceDir, const String& fileName)
{
    // #623 BEGIN TODO: Skip files that have a converted version in the cache
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();
    if (resourceDir != cachePath)
    {
        String ext = GetExtension(fileName);
        if (ext == ".jpg" || ext == ".png" || ext == ".tga")
        {
            FileSystem* fileSystem = GetSubsystem<FileSystem>();
            String compressedPath = cachePath + "DDS/" + fileName + ".dds";
            if (fileSystem->FileExists(compressedPath))
                return false;
        }
    }
    // #623 END TODO
    
    return BuildBase::CheckIncludeResourceFile(resourceDir, fileName);
}

void BuildWindows::BuildAtomicNET()
{
    // AtomicNET

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();
    String projectResources = project->GetResourcePath();

    String assembliesPath = projectResources + "Assemblies/";

    // if no assemblies path, no need to install AtomicNET
    if (!fileSystem->DirExists(assembliesPath))
        return;

    Vector<String> results;
    fileSystem->ScanDir(results, assembliesPath, "*.dll", SCAN_FILES, true);

    // if no assembiles in Assemblies path, no need to install AtomicNET
    if (!results.Size())
        return;

    BuildLog("Building AtomicNET");

    fileSystem->CreateDir(buildPath_ + "/AtomicPlayer_Resources/AtomicNET");
    fileSystem->CreateDir(buildPath_ + "/AtomicPlayer_Resources/AtomicNET/Atomic");
    fileSystem->CreateDir(buildPath_ + "/AtomicPlayer_Resources/AtomicNET/Atomic/Assemblies");

    fileSystem->CopyDir(tenv->GetNETCoreCLRAbsPath(), buildPath_ + "/AtomicPlayer_Resources/AtomicNET/CoreCLR");
    fileSystem->CopyDir(tenv->GetNETTPAPaths(), buildPath_ + "/AtomicPlayer_Resources/AtomicNET/Atomic/TPA");

    // Atomic Assemblies

    const String& assemblyLoadPaths = tenv->GetNETAssemblyLoadPaths();
    Vector<String> paths = assemblyLoadPaths.Split(';');

    for (unsigned i = 0; i < paths.Size(); i++)
    {
        Vector<String> loadResults;
        fileSystem->ScanDir(loadResults, paths[i], "*.dll", SCAN_FILES, true);

        for (unsigned j = 0; j < loadResults.Size(); j++)
        {
            String pathName, fileName, ext;
            SplitPath(loadResults[j], pathName, fileName, ext);

            if (fileName != "AtomicNETEngine")
                continue;

            fileSystem->Copy(paths[i] + "/" + loadResults[j], ToString("%s/AtomicPlayer_Resources/AtomicNET/Atomic/Assemblies/%s.dll", buildPath_.CString(), fileName.CString()));
        }

    }

    // Project assemblied
    for (unsigned i = 0; i < results.Size(); i++)
    {
        String pathName, fileName, ext;
        SplitPath(results[i], pathName, fileName, ext);
        fileSystem->Copy(assembliesPath + results[i], ToString("%s/AtomicPlayer_Resources/AtomicNET/Atomic/Assemblies/%s.dll", buildPath_.CString(), fileName.CString()));
    }



}

void BuildWindows::Build(const String& buildPath)
{
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    BuildLog("Starting Windows Deployment");

    Initialize();

    if (!BuildClean(buildPath_))
        return;

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    String rootSourceDir = tenv->GetRootSourceDir();
    String playerBinary = tenv->GetPlayerBinary();
    String d3d9dll = GetPath(playerBinary) + "/D3DCompiler_47.dll";

    if (!BuildCreateDirectory(buildPath_))
        return;

    if (!BuildCreateDirectory(buildPath_ + "/AtomicPlayer_Resources"))
        return;

    String resourcePackagePath = buildPath_ + "/AtomicPlayer_Resources/AtomicResources" + PAK_EXTENSION;
    GenerateResourcePackage(resourcePackagePath);
    if (buildFailed_)
        return;

    if (!BuildCreateDirectory(buildPath_ + "/Settings"))
        return;

    String engineJSON(GetSettingsDirectory() + "/Engine.json");

    if (fileSystem->FileExists(engineJSON))
    {
        if (!BuildCopyFile(engineJSON, buildPath_ + "/Settings/Engine.json"))
            return;
    }

    if (!BuildCopyFile(playerBinary, buildPath_ + "/AtomicPlayer.exe"))
        return;

    if (!BuildCopyFile(d3d9dll, buildPath_ + "/D3DCompiler_47.dll"))
        return;

    BuildAtomicNET();

    BuildLog("Windows Deployment Complete");

    buildSystem->BuildComplete(PLATFORMID_WINDOWS, buildPath_);

}

}
