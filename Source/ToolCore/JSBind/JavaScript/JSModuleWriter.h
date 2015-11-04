//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>

#include "../JSBModuleWriter.h"

using namespace Atomic;

namespace ToolCore
{

class JSBModule;

class JSModuleWriter : public JSBModuleWriter
{

public:

    JSModuleWriter(JSBModule* module);

    void GenerateSource();

private:

    void WriteIncludes(String& source);
    void WriteForwardDeclarations(String& source);
    void WriteClassDeclaration(String& source);

    void WriteClassDefine(String& source);
    void WriteModulePreInit(String& source);
    void WriteModuleInit(String& source);

};

}
