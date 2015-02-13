// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>
#include "Atomic/Container/Vector.h"
#include <Atomic/IO/File.h>

#include "BuildTypes.h"

using namespace Atomic;

namespace AtomicEditor
{

class BuildBase;

class ResourcePackager : public Object
{
    OBJECT(ResourcePackager);

public:

    ResourcePackager(Context* context, BuildBase* buildBase);
    virtual ~ResourcePackager();

    void AddResourceEntry(BuildResourceEntry* entry);

    void GeneratePackage(const String& destFilePath);

private:

    void WriteHeader(File* dest);
    bool WritePackageFile(const String& destFilePath);

    PODVector<BuildResourceEntry*> resourceEntries_;

    WeakPtr<BuildBase> buildBase_;

    unsigned checksum_;

};

}
