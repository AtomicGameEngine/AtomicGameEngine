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
class JSBFunction;

class CSPackageWriter : public JSBPackageWriter
{

public:

    CSPackageWriter(JSBPackage* package);

    void GenerateSource();

    void GenNativeFunctionSignature(JSBFunction* function, String& sig);

    void GenerateNativeHeader();
    void GenerateNativeSource();
    void GenerateManagedSource();

private:

};

}
