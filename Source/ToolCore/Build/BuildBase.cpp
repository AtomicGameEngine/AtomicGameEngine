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

#include "BuildEvents.h"
#include "BuildBase.h"
#include "ResourcePackager.h"

namespace ToolCore
{

BuildBase::BuildBase(Context * context, Project* project, PlatformID platform) : Object(context),
    platformID_(platform),
    containsMDL_(false)
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

void BuildBase::BuildLog(const String& message)
{
    buildLog_.Push(message);
}

void BuildBase::BuildWarn(const String& warning)
{
    buildWarnings_.Push(warning);
}

void BuildBase::BuildError(const String& error)
{
    buildErrors_.Push(error);
}

void BuildBase::SendBuildFailure(const String& message)
{

    VariantMap buildError;
    buildError[BuildComplete::P_PLATFORMID] = platformID_;
    buildError[BuildComplete::P_MESSAGE] = message;
    SendEvent(E_BUILDCOMPLETE, buildError);

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

        // TODO: Add additional filters
        if (GetExtension(filename) == ".psd")
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
