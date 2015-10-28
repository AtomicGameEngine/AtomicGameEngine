#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

namespace ToolCore
{

class JSBPackage;
class JSBClass;

class JSBClassWriter
{

public:

    JSBClassWriter(JSBClass* klass);

    void GenerateSource(String& sourceOut);

private:

    void WriteFunctions(String& source);
    void GenerateStaticFunctionsSource(String& source, String& packageName);
    void GenerateNonStaticFunctionsSource(String& source, String& packageName);

    JSBClass* klass_;

};

}
