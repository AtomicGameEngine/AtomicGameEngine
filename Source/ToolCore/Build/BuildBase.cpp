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
    project_(project),
    containsMDL_(false),
    buildFailed_(false),
    assetBuildTag_(String::EMPTY),
    fileIncludedResourcesLog_(nullptr)
{
    if (UseResourcePackager())
        resourcePackager_ = new ResourcePackager(context, this);

    fileIncludedResourcesLog_ = new File(context_, "BuildIncludedResources.log", Atomic::FILE_WRITE);

    ReadAssetBuildConfig();
}

BuildBase::~BuildBase()
{
    for (unsigned i = 0; i < resourceEntries_.Size(); i++)
    {
        delete resourceEntries_[i];
    }

    fileIncludedResourcesLog_->Close();
    delete fileIncludedResourcesLog_;
}

#ifdef ATOMIC_PLATFORM_WINDOWS

bool BuildBase::BuildClean(const String& path)
{
    if (buildFailed_)
    {
        ATOMIC_LOGERRORF("BuildBase::BuildClean - Attempt to clean directory of failed build, %s", path.CString());
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
            ATOMIC_LOGWARNINGF("BuildBase::BuildClean - temp build folder exists, removing: %s", newPath.CString());
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
        ATOMIC_LOGERRORF("BuildBase::BuildCreateDirectory - Attempt to create directory of failed build, %s", path.CString());
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
        ATOMIC_LOGERRORF("BuildBase::BuildCopyFile - Attempt to copy file of failed build, %s", srcFileName.CString());
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
        ATOMIC_LOGERRORF("BuildBase::BuildCopyDir - Attempt to copy directory of failed build, %s", srcDir.CString());
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
        ATOMIC_LOGERRORF("BuildBase::BuildRemoveDirectory - Attempt to remove directory of failed build, %s", path.CString());
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
        ATOMIC_LOGERRORF("BuildBase::FailBuild - Attempt to fail already failed build: %s", message.CString());
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

void BuildBase::BuildDefaultResourceEntries()
{
    String buildLogOutput(String::EMPTY);

    for (unsigned i = 0; i < resourceDirs_.Size(); i++)
    {
        String resourceDir = resourceDirs_[i];
        fileIncludedResourcesLog_->WriteLine("\nBuildBase::BuildDefaultResourceEntries - Default resources being included from: " + resourceDir);

        Vector<String> fileNames;
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        fileSystem->ScanDir(fileNames, resourceDir, "*.*", SCAN_FILES, true);

        for (unsigned i = 0; i < fileNames.Size(); i++)
        {
            const String& filename = fileNames[i];

            AddToResourcePackager(filename, resourceDir);
        }
    }
}

void BuildBase::BuildProjectResourceEntries()
{
    String buildLogOutput(String::EMPTY);

    if (AssetBuildConfig::IsLoaded())
    {
        buildLogOutput += "\nBaseBuild::BuildProjectResourceEntries - ./Settings/AssetBuildConfig.json found. ";
        if (!assetBuildTag_.Empty())
        {
            buildLogOutput += "Using build-tag parameter :  " + assetBuildTag_;
            fileIncludedResourcesLog_->WriteLine(buildLogOutput);

            BuildFilteredProjectResourceEntries();
            return;
        }
        buildLogOutput += "No build-tag parameter used.";
    }
    buildLogOutput += "\nBaseBuild::BuildProjectResourceEntries - No custom asset include configuration being used - ";
    buildLogOutput += "AssetBuildConfig.json not loaded, OR no build-tag parameter used.";
    fileIncludedResourcesLog_->WriteLine(buildLogOutput);

    BuildAllProjectResourceEntries();
}

void BuildBase::BuildAllProjectResourceEntries()
{
    for (unsigned i = 0; i < projectResourceDir_.Size(); i++)
    {
        String projectResourceDir = projectResourceDir_[i];
        fileIncludedResourcesLog_->WriteLine("\nBuildBase::BuildAllProjectResourceEntries - Project resources being included from: " + projectResourceDir);

        Vector<String> fileNamesInProject;
        FileSystem* fileSystem = GetSubsystem<FileSystem>();
        fileSystem->ScanDir(fileNamesInProject, projectResourceDir, "*.*", SCAN_FILES, true);

        for (unsigned i = 0; i < fileNamesInProject.Size(); i++)
        {
            AddToResourcePackager(fileNamesInProject[i], projectResourceDir);
        }
    }
}

void BuildBase::BuildFilteredProjectResourceEntries()
{
    // Loading up the AssetBuildConfig.json,
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
            for (unsigned i = 0; i < assetBuildConfigFiles.Size(); ++i)
            {
                // remove case sensitivity
                assetBuildConfigFiles[i] = assetBuildConfigFiles[i].ToLower();
            }
            break;
        }

        itr++;
        if (itr == resourceTags.End())
        {
            ATOMIC_LOGERRORF("BuildBase::BuildFilteredProjectResourceEntries - Asset build-tag \"%s\" not defined in ./Settings/AssetBuildConfig.json", assetBuildTag_.CString());
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
            // rename 'filename' to 'folder' for context
            String folder(filename);

            // add a trailing slash if not defined
            if (folder.Back() != '/')
                folder = AddTrailingSlash(folder);

            Vector<String> filesInFolder;
            fileSystem->ScanDir(filesInFolder, project_->GetResourcePath() + folder, "*.*", SCAN_FILES, true);
            for (unsigned j = 0; j < filesInFolder.Size(); ++j)
            {
                String path = filesInFolder[j];

                // not interested in .asset files for now, will be included later.
                if (GetExtension(path) != ".asset")
                {
                    filesInFolderToAdd.Push(folder + path.ToLower());
                }
            }
        }
    }
    // add the files defined using a folder in AssetBuildConfig.json
    for (unsigned i = 0; i < filesInFolderToAdd.Size(); ++i)
    {
        assetBuildConfigFiles.Push(filesInFolderToAdd[i]);
    }

    // check if the files in AssetBuildConfig.json exist,
    // as well as their corresponding .asset file
    Vector<String> filesInResourceFolder;
    Vector<String> resourceFilesToInclude;
    fileSystem->ScanDir(filesInResourceFolder, project_->GetResourcePath(), "*.*", SCAN_FILES, true);
    for (unsigned j = 0; j < filesInResourceFolder.Size(); ++j)
    {
        // don't want to checks to be case sensitive
        filesInResourceFolder[j] = filesInResourceFolder[j].ToLower();
    }

    for (unsigned i = 0; i < assetBuildConfigFiles.Size(); ++i)
    {
        // .asset file is of primary importance since we used it to identify the associated cached file.
        // without the .asset file the resource is removed from being included in the build.

        // don't want checks to be case sensitive
        String &filename = assetBuildConfigFiles[i];
        if (filesInResourceFolder.Contains(filename) &&
            filesInResourceFolder.Contains(filename + ".asset"))
        {
            resourceFilesToInclude.Push(filename);
            resourceFilesToInclude.Push(filename + ".asset");
            continue;
        }
        fileIncludedResourcesLog_->WriteLine("File " + filename + " ignored since it does not have an associated .asset file");
    }

    // add valid files included from the AssetBuildConfig.json
    for (StringVector::ConstIterator it = resourceFilesToInclude.Begin(); it != resourceFilesToInclude.End(); ++it)
    {
        AddToResourcePackager(*it, project_->GetResourcePath());
    }

    // Get associated cache GUID from the asset file
    Vector<String> filesWithGUIDtoInclude;

    for (StringVector::Iterator it = resourceFilesToInclude.Begin(); it != resourceFilesToInclude.End(); ++it)
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
    // Check if the file contains the guid, and add it to the cacheFilesToInclude
    Vector<String> filesInCacheFolder;
    Vector<String> cacheFilesToInclude;
    AssetDatabase* db = GetSubsystem<AssetDatabase>();
    String cachePath = db->GetCachePath();
    fileSystem->ScanDir(filesInCacheFolder, cachePath, "*.*", SCAN_FILES, false);
    for (unsigned i = 0; i < filesWithGUIDtoInclude.Size(); ++i)
    {
        String &guid = filesWithGUIDtoInclude[i];
        for (unsigned j = 0; j < filesInCacheFolder.Size(); ++j)
        {
            const String &filename = GetFileName(filesInCacheFolder[j]);
            String &filenamePath = filesInCacheFolder[j];
            if (filename.Contains(guid))
            {
                cacheFilesToInclude.Push(filesInCacheFolder[j]);
                // do not continue...
                // there might be multiple files with the same guid having an guid_animaiton extention.
            }
        }
    }

    // include a texture file's cached .dds file when building in windows
#ifdef ATOMIC_PLATFORM_DESKTOP
    for (StringVector::ConstIterator it = resourceFilesToInclude.Begin(); it != resourceFilesToInclude.End(); ++it)
    {
        if (!CheckIncludeResourceFile(project_->GetResourcePath(), *it))
        {
            FileSystem* fileSystem = GetSubsystem<FileSystem>();
            String associatedDDSpath = "DDS/" + *it + ".dds";
            String compressedPath = cachePath + associatedDDSpath;
            if (fileSystem->FileExists(compressedPath))
            {
                cacheFilesToInclude.Push(associatedDDSpath);
            }
        }
    }
#endif

    // Add the cache files to the resource packager
    for (StringVector::ConstIterator it = cacheFilesToInclude.Begin(); it != cacheFilesToInclude.End(); ++it)
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

    if (!CheckIncludeResourceFile(resourceDir, filename))
    {
        fileIncludedResourcesLog_->WriteLine(resourceDir + filename + " skipped because of file extention: " + GetExtension(filename));
        return;
    }

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

    fileIncludedResourcesLog_->WriteLine(newEntry->absolutePath_);
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
