//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>

#include "../JSBClassWriter.h"

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBClass;
class JSBFunction;

class CSClassWriter : public JSBClassWriter
{

public:

    CSClassWriter(JSBClass* klass);

    void GenerateSource(String& sourceOut);

    void GenerateNativeSource(String& sourceOut);
    void GenerateManagedSource(String& sourceOut);

private:

    void WriteNativeFunctions(String& source);

    void WriteManagedProperties(String& sourceOut);
    void WriteManagedFunctions(String& source);
};

}
