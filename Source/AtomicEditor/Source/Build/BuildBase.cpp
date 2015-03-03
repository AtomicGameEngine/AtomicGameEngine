// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/IO/Log.h>
#include <Atomic/IO/FileSystem.h>

#include "BuildBase.h"
#include "ResourcePackager.h"

namespace AtomicEditor
{

BuildBase::BuildBase(Context * context) : Object(context), containsMDL_(false)
{

    if (UseResourcePackager())
        resourcePackager_ = new ResourcePackager(context, this);

    BuildLog("Build Started");

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

        BuildResourceEntry* newEntry = new BuildResourceEntry;

// BEGIN LICENSE MANAGEMENT
        if (GetExtension(filename) == ".mdl")
        {
            containsMDL_ = true;
        }
// END LICENSE MANAGEMENT

        newEntry->absolutePath_ = resourceDir + filename;
        newEntry->packagePath_ = filename;
        newEntry->resourceDir_ = resourceDir;

        resourceEntries_.Push(newEntry);
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
