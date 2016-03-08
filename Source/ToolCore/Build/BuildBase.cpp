//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "../Subprocess/SubprocessSystem.h"
#include "../Project/Project.h"
#include "../ToolEnvironment.h"

#include "BuildSystem.h"
#include "BuildEvents.h"
#include "BuildBase.h"
#include "ResourcePackager.h"

namespace ToolCore
{

BuildBase::BuildBase(Context * context, Project* project, PlatformID platform) : Object(context),
    platformID_(platform),
    containsMDL_(false),
    buildFailed_(false)
{
    if (UseResourcePackager())
        resourcePackager_ = new ResourcePackager(context, this);

    project_ = project;

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

    bool result = fileSystem->RemoveDir(path, true);

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
    Vector<String> fileNames;
    FileSystem* fileSystem = GetSubsystem<FileSystem>();
    fileSystem->ScanDir(fileNames, resourceDir, "*.*", SCAN_FILES, true);

    for (unsigned i = 0; i < fileNames.Size(); i++)
    {
        const String& filename = fileNames[i];

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
            continue;

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

        //LOGINFOF("Adding resource: %s : %s", newEntry->absolutePath_.CString(), newEntry->packagePath_.CString());
    }
}

void BuildBase::BuildResourceEntries()
{
    for (unsigned i = 0; i < resourceDirs_.Size(); i++)
    {
        ScanResourceDirectory(resourceDirs_[i]);
    }

    if (resourcePackager_.NotNull())
    {
        for (unsigned i = 0; i < resourceEntries_.Size(); i++)
        {
            BuildResourceEntry* entry = resourceEntries_[i];
            resourcePackager_->AddResourceEntry(entry);
        }

    }

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


}
