
#pragma once

#include "../Container/Str.h"

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
