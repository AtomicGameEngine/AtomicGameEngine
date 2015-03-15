// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

class JSBFunction;

class JSBDoc
{

    String source_;

    void Begin();

    void End();

    String GenFunctionDoc(JSBFunction* function);

    void ExportModuleEnums(const String& moduleName);
    void ExportModuleConstants(const String& moduleName);
    void ExportModuleClasses(const String& moduleName);

    void WriteToFile(const String& path);


public:

    void Emit(const String& path);

};
