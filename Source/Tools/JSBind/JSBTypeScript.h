// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicEditor

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

class JSBFunction;

class JSBTypeScript
{

    String source_;

    void Begin();

    void End();

    void ExportFunction(JSBFunction* function);

    void ExportModuleEnums(const String& moduleName);
    void ExportModuleConstants(const String& moduleName);
    void ExportModuleClasses(const String& moduleName);

    void WriteToFile(const String& path);


public:

    void Emit(const String& path);

};
