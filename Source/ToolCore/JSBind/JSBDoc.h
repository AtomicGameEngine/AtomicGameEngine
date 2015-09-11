//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

namespace ToolCore
{

class JSBFunction;
class JSBPackage;
class JSBModule;

class JSBDoc
{

public:

    void Emit(JSBPackage* package, const String& path);

private:

    void Begin();

    void End();

    String GenFunctionDoc(JSBFunction* function);

    void ExportModuleEnums(JSBModule* module);
    void ExportModuleConstants(JSBModule* module);
    void ExportModuleClasses(JSBModule* module);

    void WriteToFile(const String& path);

    String source_;
    JSBPackage* package_;

};

}
