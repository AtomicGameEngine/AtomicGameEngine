//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>

#include "../JSBPackageWriter.h"

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBClass;

class JSPackageWriter : public JSBPackageWriter
{

public:

    JSPackageWriter(JSBPackage* package);

    void GenerateSource();

    virtual void PostProcess();

private:

    void WriteProtoTypeRecursive(String &source, JSBClass* klass,  Vector<JSBClass*>& written);
    void WriteProtoTypeSetup(String& source);

};

}
