//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

class JSBFunction;
class JSBPackage;
class JSBModule;

namespace ToolCore
{

class JSBFunctionType;

class JSBTypeScript
{

public:

    void Emit(JSBPackage* package, const String& path);

private:

    String source_;

    void Begin();

    void End();

    void ExportFunction(JSBFunction* function);

    String GetScriptType(JSBFunctionType* ftype);

    void ExportModuleEnums(JSBModule* moduleName);
    void ExportModuleConstants(JSBModule*  moduleName);
    void ExportModuleClasses(JSBModule*  moduleName);

    void WriteToFile(const String& path);

    JSBPackage* package_;


};

}
