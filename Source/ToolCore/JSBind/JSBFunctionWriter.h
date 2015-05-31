#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBFunction;

class JSBFunctionWriter
{

public:

    JSBFunctionWriter(JSBFunction* function);

    void GenerateSource(String& sourceOut);

private:

    void WriteFunction(String& source);
    void WriteConstructor(String& source);
    void WriteParameterMarshal(String& source);

    JSBFunction* function_;

};

}
