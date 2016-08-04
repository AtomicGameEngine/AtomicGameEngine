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
#include <Atomic/IO/Log.h>
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
    
    for (unsigned i = 0; i < defaultResourcePaths.Size(); i++)
    {
        AddResourceDir(defaultResourcePaths[i]);
    }
    BuildDefaultResourceEntries();
    
    // Include the project resources and cache separately
    AddProjectResourceDir(project->GetResourcePath());
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();
    AddProjectResourceDir(cachePath);

    BuildProjectResourceEntries();
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

void BuildWindows::BuildManaged(const String& buildPath)
{
    BuildLog("Building Managed Application");

    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* toolSystem = GetSubsystem<ToolSystem>();
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    Project* project = toolSystem->GetProject();
    
    StringVector results;
    StringVector filtered;
    fileSystem->ScanDir(results, tenv->GetAtomicNETCoreAssemblyDir(), "", SCAN_FILES, false);

    StringVector filterList;

    filterList.Push("AtomicIPCPlayer");
    filterList.Push(".pdb");
    filterList.Push("System.Collections.Immutable");
    filterList.Push("System.Reflection.Metadata");
    filterList.Push("ComponentTest");
    filterList.Push("AtomicNETService");

    StringVector::Iterator itr = results.Begin();
    while (itr != results.End())
    {
        unsigned i;
        for (i = 0; i < filterList.Size(); i++)
        {
            if (itr->Contains(filterList[i]))
                break;
        }

        if (i == filterList.Size())
            filtered.Push(*itr);

        itr++;
    }

    String playerBinary = tenv->GetAtomicNETManagedPlayerBinary();
   
    if (!BuildCopyFile(playerBinary, buildPath_ + "/AtomicPlayer.exe"))
        return;

    for (unsigned i = 0; i < filtered.Size(); i++)
    {
        String filename = filtered[i];

        if (!BuildCopyFile(tenv->GetAtomicNETCoreAssemblyDir() + filename, buildPath_ + "/" + filename))
            return;

    }
    if (!BuildCopyFile(project->GetResourcePath() + "/AtomicProject.dll", buildPath_ + "/AtomicProject.dll"))
        return;

   
  
}

void BuildWindows::BuildNative(const String& buildPath)
{
    BuildLog("Building Native Application");

    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

    String playerBinary = tenv->GetPlayerBinary();

    String d3d9dll = GetPath(playerBinary) + "/D3DCompiler_47.dll";

    if (!BuildCopyFile(playerBinary, buildPath_ + "/AtomicPlayer.exe"))
        return;

    if (!BuildCopyFile(d3d9dll, buildPath_ + "/D3DCompiler_47.dll"))
        return;

}


void BuildWindows::Build(const String& buildPath)
{
    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();
    ToolSystem* tsystem = GetSubsystem<ToolSystem>();
    Project* project = tsystem->GetProject();

    buildPath_ = AddTrailingSlash(buildPath) + GetBuildSubfolder();

    BuildLog("Starting Windows Deployment");

    Initialize();

    if (!BuildClean(buildPath_))
        return;

    String rootSourceDir = tenv->GetRootSourceDir();        

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

    // TODO: Set project as managed and don't key off AtomicProject.dll

    if (fileSystem->FileExists(project->GetResourcePath() + "/AtomicProject.dll"))
    {
        BuildManaged(buildPath);
    }
    else
    {
        BuildNative(buildPath);
    }
    
    BuildLog("Windows Deployment Complete");

    buildSystem->BuildComplete(PLATFORMID_WINDOWS, buildPath_);

}

}
