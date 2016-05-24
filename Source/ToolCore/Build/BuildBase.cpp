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
#include <Poco/File.h>

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>
#include <Atomic/Resource/JSONFile.h>

#include "../Subprocess/SubprocessSystem.h"
#include "../Project/Project.h"
#include "../ToolEnvironment.h"
#include "../Assets/Asset.h"
#include "../Assets/AssetDatabase.h"

#include "BuildSystem.h"
#include "BuildEvents.h"
#include "BuildBase.h"
#include "ResourcePackager.h"
#include "AssetBuildConfig.h"

namespace ToolCore
{

BuildBase::BuildBase(Context * context, Project* project, PlatformID platform) : Object(context),
    platformID_(platform),
    containsMDL_(false),
    buildFailed_(false),
    assetBuildTag_(String::EMPTY)
{
    if (UseResourcePackager())
        resourcePackager_ = new ResourcePackager(context, this);

    project_ = project;

    ReadAssetBuildConfig();
}

BuildBase::~BuildBase()
{
    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        delete resourceEntries_[i];
    }
}

#ifdef ATOMIC_PLATFORM_WINDOWS

bool BuildBase::BuildClean(const String& path)
{
    if (buildFailed_)
    {
        LOGERRORF("BuildBase::BuildClean - Attempt to clean directory of failed build, %s", path.CString());
        return false;
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (!fileSystem->DirExists(path))
        return true;

    // On Windows, do a little dance with the folder to avoid issues
    // with deleting folder and immediately recreating it

    String pathName, fileName, ext;
    SplitPath(path, pathName, fileName, ext);
    pathName = AddTrailingSlash(pathName);

    unsigned i = 0;
    while (true)
    {
        String newPath = ToString("%s%s_Temp_%u", pathName.CString(), fileName.CString(), i++);
        if (!fileSystem->DirExists(newPath))
        {
            if (!MoveFileExW(GetWideNativePath(path).CString(), GetWideNativePath(newPath).CString(), MOVEFILE_WRITE_THROUGH))
            {
                FailBuild(ToString("BuildBase::BuildClean: Unable to move directory %s -> ", path.CString(), newPath.CString()));
                return false;
            }

            // Remove the moved directory
            return BuildRemoveDirectory(newPath);

        }
        else
        {
            LOGWARNINGF("BuildBase::BuildClean - temp build folder exists, removing: %s", newPath.CString());
            fileSystem->RemoveDir(newPath, true);
        }

        if (i == 255)
        {
            FailBuild(ToString("BuildBase::BuildClean: Unable to move directory ( i == 255) %s -> ", path.CString(), newPath.CString()));
            return false;
        }
    }

    return false;
}

#else

bool BuildBase::BuildClean(const String& path)
{
    return BuildRemoveDirectory(path);
}

#endif

bool BuildBase::BuildCreateDirectory(const String& path)
{
    if (buildFailed_)
    {
        LOGERRORF("BuildBase::BuildCreateDirectory - Attempt to create directory of failed build, %s", path.CString());
        return false;
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    if (fileSystem->DirExists(path))
        return true;

    bool result = fileSystem->CreateDir(path);

    if (!result)
    {
        FailBuild(ToString("BuildBase::BuildCreateDirectory: Unable to create directory %s", path.CString()));
        return false;
    }

    return true;

}

bool BuildBase::BuildCopyFile(const String& srcFileName, const String& destFileName)
{
    if (buildFailed_)
    {
        LOGERRORF("BuildBase::BuildCopyFile - Attempt to copy file of failed build, %s", srcFileName.CString());
        return false;
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    bool result = fileSystem->Copy(srcFileName, destFileName);

    if (!result)
    {
        FailBuild(ToString("BuildBase::BuildCopyFile: Unable to copy file %s -> %s", srcFileName.CString(), destFileName.CString()));
        return false;
    }

    return true;
}

bool BuildBase::BuildCopyDir(const String& srcDir, const String& destDir)
{
    if (buildFailed_)
    {
        LOGERRORF("BuildBase::BuildCopyDir - Attempt to copy directory of failed build, %s", srcDir.CString());
        return false;
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();

    bool result = fileSystem->CopyDir(srcDir, destDir);

    if (!result)
    {
        FailBuild(ToString("BuildBase::BuildCopyDir: Unable to copy dir %s -> %s", srcDir.CString(), destDir.CString()));
        return false;
    }

    return true;
}

bool BuildBase::CheckIncludeResourceFile(const String & resourceDir, const String & fileName)
{
    return (GetExtension(fileName) != ".psd");
}

bool BuildBase::BuildRemoveDirectory(const String& path)
{
    if (buildFailed_)
    {
        LOGERRORF("BuildBase::BuildRemoveDirectory - Attempt to remove directory of failed build, %s", path.CString());
        return false;
    }

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (!fileSystem->DirExists(path))
        return true;

#ifdef ATOMIC_PLATFORM_LINUX
    bool result = true;   // fileSystem->RemoveDir(path, true); crashes on linux
    Poco::File dirs(buildPath_.CString());
    dirs.remove(true);
    if (fileSystem->DirExists(buildPath_))
        result = false;
#else
    bool result = fileSystem->RemoveDir(path, true);
#endif

    if (!result)
    {
        FailBuild(ToString("BuildBase::BuildRemoveDirectory: Unable to remove directory %s", path.CString()));
        return false;
    }

    return true;
}

void BuildBase::BuildLog(const String& message, bool sendEvent)
{
    buildLog_.Push(message);

    if (sendEvent)
    {
        String colorMsg = ToString("<color #D4FB79>%s</color>\n", message.CString());
        VariantMap buildOutput;
        buildOutput[BuildOutput::P_TEXT] = colorMsg;
        SendEvent(E_BUILDOUTPUT, buildOutput);
    }

}

void BuildBase::BuildWarn(const String& warning, bool sendEvent)
{
    buildWarnings_.Push(warning);

    if (sendEvent)
    {
        String colorMsg = ToString("<color #FFFF00>%s</color>\n", warning.CString());
        VariantMap buildOutput;
        buildOutput[BuildOutput::P_TEXT] = colorMsg;
        SendEvent(E_BUILDOUTPUT, buildOutput);
    }

}

void BuildBase::BuildError(const String& error, bool sendEvent)
{
    buildErrors_.Push(error);

    if (sendEvent)
    {
        String colorMsg = ToString("<color #FF0000>%s</color>\n", error.CString());
        VariantMap buildOutput;
        buildOutput[BuildOutput::P_TEXT] = colorMsg;
        SendEvent(E_BUILDOUTPUT, buildOutput);
    }
}

void BuildBase::FailBuild(const String& message)
{
    if (buildFailed_)
    {
        LOGERRORF("BuildBase::FailBuild - Attempt to fail already failed build: %s", message.CString());
        return;
    }

    buildFailed_ = true;

    BuildError(message);

    BuildSystem* buildSystem = GetSubsystem<BuildSystem>();
    buildSystem->BuildComplete(platformID_, buildPath_, false, message);

}


void BuildBase::HandleSubprocessOutputEvent(StringHash eventType, VariantMap& eventData)
{
    // E_SUBPROCESSOUTPUT
    const String& text = eventData[SubprocessOutput::P_TEXT].GetString();

    // convert to a build output event and forward to subscribers
    VariantMap buildOutputData;
    buildOutputData[BuildOutput::P_TEXT] = text;
    SendEvent(E_BUILDOUTPUT, buildOutputData);
}

void BuildBase::GetDefaultResourcePaths(Vector<String>& paths)
{
    paths.Clear();

    ToolEnvironment* tenv = GetSubsystem<ToolEnvironment>();

    paths.Push(AddTrailingSlash(tenv->GetCoreDataDir()));
    paths.Push(AddTrailingSlash(tenv->GetPlayerDataDir()));
}

String BuildBase::GetSettingsDirectory()
{
    return project_->GetProjectPath() + "/Settings";
}

void BuildBase::ScanResourceDirectory(const String& resourceDir)
{
    {

        //LOGINFOF("Adding resource: %s : %s", newEntry->absolutePath_.CString(), newEntry->packagePath_.CString());
    }
}

void BuildBase::BuildDefaultResourceEntries()
{
    File file(context_, "BuildResource.log", Atomic::FILE_APPEND);
    if (!file.IsOpen())
    {
        file.Open("BuildResource.log", Atomic::FILE_APPEND);
    }
    file.WriteLine("*********************************************************");

    for (unsigned i = 0; i < resourceDirs_.Size(); i++)
    {
        String resourceDir = resourceDirs_[i];
        Vector<String> fileNames;
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        fileSystem->ScanDir(fileNames, resourceDir, "*.*", SCAN_FILES, true);

        for (unsigned i = 0; i < fileNames.Size(); i++)
        {
            const String& filename = fileNames[i];
            
            if (!CheckIncludeResourceFile(resourceDir, filename))
                continue;

            String fileInfo = resourceDir + filename;
            file.WriteLine(fileInfo.CString());
            AddToResourcePackager(filename, resourceDir);
        }
    }
    file.Close();
}

void BuildBase::BuildProjectResourceEntries()
{
    if (AssetBuildConfig::IsLoaded() && !assetBuildTag_.Empty())
    {
        // add log comment
        BuildFilteredProjectResourceEntries();
    }
    else
    {
        // add log comment
        BuildAllProjectResourceEntries();
    }
}

void BuildBase::BuildAllProjectResourceEntries()
{
    File file(context_, "BuildResource.log", Atomic::FILE_APPEND);
    if (!file.IsOpen())
    {
        file.Open("BuildResource.log", Atomic::FILE_APPEND);
    }
    file.WriteLine("*********************************************************");
    for (unsigned i = 0; i < projectResourceDir_.Size(); i++)
    {
        String projectResourceDir = projectResourceDir_[i];
        Vector<String> fileNamesInProject;
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        fileSystem->ScanDir(fileNamesInProject, projectResourceDir, "*.*", SCAN_FILES, true);

        for (unsigned i = 0; i < fileNamesInProject.Size(); i++)
        {
            String fileInfo = projectResourceDir + fileNamesInProject[i];
            file.WriteLine(fileInfo.CString());
            AddToResourcePackager(fileNamesInProject[i], projectResourceDir);
        }
    }
    file.Close();
}

void BuildBase::BuildFilteredProjectResourceEntries()
{
    // Loading up the assetbuildconfig.json,
    // obtaining a list of files to include in the build.
    VariantMap resourceTags;
    AssetBuildConfig::ApplyConfig(resourceTags);
    Vector<String> assetBuildConfigFiles;
    VariantMap::ConstIterator itr = resourceTags.Begin();

    while (itr != resourceTags.End())
    {
        if (itr->first_ == assetBuildTag_)
        {
            assetBuildConfigFiles = itr->second_.GetStringVector();
            break;
        }
        
        itr++;
        if (itr == resourceTags.End())
        {
            LOGERRORF("BuildBase::BuildFilteredProjectResourceEntries - Asset Build Tag \"%s\" not defined in .\\Settings\\assetbuildconfig.json", assetBuildTag_.CString());
        }
    }

    // find any folders defined in assetbuildconfig.json, and add the non-".asset" files in them.
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    Vector<String> filesInFolderToAdd;
    
    for (unsigned i = 0; i < assetBuildConfigFiles.Size(); ++i)
    {
        String &filename = assetBuildConfigFiles[i];
        if (GetExtension(filename) == String::EMPTY &&
            fileSystem->DirExists(project_->GetResourcePath() + filename))
        {
            Vector<String> filesInFolder;
            fileSystem->ScanDir(filesInFolder, project_->GetResourcePath() + filename, "*.*", SCAN_FILES, true);
            for (unsigned j = 0; j < filesInFolder.Size(); ++j)
            {
                if (GetExtension(filesInFolder[j]) != ".asset")
                    filesInFolderToAdd.Push(filesInFolder[j]);
            }
        }
    }

    // add the files defined using a folder in assetbuildconfig.json
    for (unsigned i = 0; i < filesInFolderToAdd.Size(); ++i)
    {
        assetBuildConfigFiles.Push(filesInFolderToAdd[i]);
    }

    // check if the files in assetbuildconfig.json exist,
    // as well as their corresponding .asset file
    Vector<String> filesInResourceFolder;
    Vector<String> resourceFilesToInclude;
    fileSystem->ScanDir(filesInResourceFolder, project_->GetResourcePath(), "*.*", SCAN_FILES, true);

    for (unsigned i = 0; i < assetBuildConfigFiles.Size(); ++i)
    {
        // .asset file is of primary importance since we used it to identify the associated cached file.
        // without the .asset file the resource is removed from being included in the build.
        String &filename = assetBuildConfigFiles[i];
        if (filesInResourceFolder.Contains(filename) &&
            filesInResourceFolder.Contains(filename + ".asset"))
        {
            resourceFilesToInclude.Push(filename);
            resourceFilesToInclude.Push(filename + ".asset");
        }
    }

    // add valid files included from the assetbuildconfig.json
    for (auto it = resourceFilesToInclude.Begin(); it != resourceFilesToInclude.End(); ++it)
    {
        AddToResourcePackager(*it, project_->GetResourcePath());
    }

    // Get associated cache GUID from the asset file
    Vector<String> filesWithGUIDtoInclude;

    for (auto it = resourceFilesToInclude.Begin(); it != resourceFilesToInclude.End(); ++it)
    {
        String &filename = *it;
        if (GetExtension(*it) == ".asset")
        {
            SharedPtr<File> file(new File(context_, project_->GetResourcePath() + *it));
            SharedPtr<JSONFile> json(new JSONFile(context_));
            json->Load(*file);
            file->Close();

            JSONValue root = json->GetRoot();
            int test = root.Get("version").GetInt();
            assert(root.Get("version").GetInt() == ASSET_VERSION);

            String guid = root.Get("guid").GetString();
            filesWithGUIDtoInclude.Push(guid);
        }
    }
    
    // Obtain files in cache folder,
    // Check if the file contains the guid, and add it to the resourceFilesToInclude
    Vector<String> filesInCacheFolder;
    Vector<String> cacheFilesToInclude;
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();
    fileSystem->ScanDir(filesInCacheFolder, cachePath, "*.*", SCAN_FILES, true);

    for (unsigned i = 0; i < filesWithGUIDtoInclude.Size(); ++i)
    {
        String &guid = filesWithGUIDtoInclude[i];
        for (unsigned j = 0; j < filesInCacheFolder.Size(); ++j)
        {
            String &filename = GetFileName(filesInCacheFolder[j]);
            if (filename.Contains(guid))
            {
                cacheFilesToInclude.Push(filesInCacheFolder[j]);
                // do not continue...
                // there might be multiple files with the same guid having an guid_animaiton extention.
            }
        }
    }

    // Add the DDS files when building in windows
#ifdef ATOMIC_PLATFORM_DESKTOP
    Vector<String> filesInCacheDDSfolder;
    fileSystem->ScanDir(filesInCacheDDSfolder, cachePath + "DDS/", "*.dds", SCAN_FILES, true);

    for (unsigned i = 0; i < filesInCacheDDSfolder.Size(); ++i)
    {
        cacheFilesToInclude.Push(filesInCacheDDSfolder[i]);
    }
#endif

    // Add the cache files to the resource packager
    for (auto it = cacheFilesToInclude.Begin(); it != cacheFilesToInclude.End(); ++it)
    {
        AddToResourcePackager(*it, cachePath);
    }
}

void BuildBase::AddToResourcePackager(const String& filename, const String& resourceDir)
{
    // Check if the file is already included in the resourceEntries_ list
    for (unsigned j = 0; j < resourceEntries_.Size(); j++)
    {
        const BuildResourceEntry* entry = resourceEntries_[j];

        if (entry->packagePath_ == filename)
        {
            BuildWarn(ToString("Resource Path: %s already exists", filename.CString()));
            continue;
        }
    }

    // Add the file to the resourceEntries_ list
    // TODO: Add additional filters
    if (CheckIncludeResourceFile(resourceDir, filename))
        return;

    //if (GetExtension(filename) == ".psd")
    //    return;

    BuildResourceEntry* newEntry = new BuildResourceEntry;

    // BEGIN LICENSE MANAGEMENT
    if (GetExtension(filename) == ".mdl")
    {
        containsMDL_ = true;
    }
    // END LICENSE MANAGEMENT

    newEntry->absolutePath_ = resourceDir + filename;
    newEntry->resourceDir_ = resourceDir;

    newEntry->packagePath_ = filename;

    resourceEntries_.Push(newEntry);
    
    assert(resourcePackager_.NotNull());
    resourcePackager_->AddResourceEntry(newEntry);
}

void BuildBase::GenerateResourcePackage(const String& resourcePackagePath)
{
    resourcePackager_->GeneratePackage(resourcePackagePath);
}

void BuildBase::AddResourceDir(const String& dir)
{
    assert(!resourceDirs_.Contains(dir));
    resourceDirs_.Push(dir);
}

void BuildBase::AddProjectResourceDir(const String& dir)
{
    assert(!projectResourceDir_.Contains(dir));
    projectResourceDir_.Push(dir);
}

void BuildBase::ReadAssetBuildConfig()
{
    String projectPath = project_->GetProjectPath();
    projectPath = RemoveTrailingSlash(projectPath);

    String filename = projectPath + "Settings/AssetBuildConfig.json";

    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    if (!fileSystem->FileExists(filename))
        return;

    if (AssetBuildConfig::LoadFromFile(context_, filename))
    {
        VariantMap assetBuildConfig;
        AssetBuildConfig::ApplyConfig(assetBuildConfig);
    }
}


}
