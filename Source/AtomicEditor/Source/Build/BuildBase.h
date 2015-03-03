// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>
#include "BuildTypes.h"

using namespace Atomic;

namespace AtomicEditor
{

class ResourcePackager;

class BuildBase : public Object
{
    OBJECT(BuildBase);

public:

    BuildBase(Context* context);
    virtual ~BuildBase();

    virtual void Build(const String& buildPath) = 0;

    // some platforms may want to do their own packaging of resources
    virtual bool UseResourcePackager() { return true; }

    // add in search order, first added is first searched
    // will warn on name conflicts
    void AddResourceDir(const String& dir);

    void BuildLog(const String& message);
    void BuildWarn(const String& warning);
    void BuildError(const String& error);

protected:

    void GenerateResourcePackage(const String& resourcePackagePath);

    void BuildResourceEntries();

    String buildPath_;
    PODVector<BuildResourceEntry*> resourceEntries_;

    bool containsMDL_;

private:

    Vector<String> buildLog_;
    Vector<String> buildWarnings_;
    Vector<String> buildErrors_;

    void ScanResourceDirectory(const String& resourceDir);

    SharedPtr<ResourcePackager> resourcePackager_;
    Vector<String> resourceDirs_;    

};

}
