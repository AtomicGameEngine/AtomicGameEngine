#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

namespace ToolCore
{

class JSBModule;

class JSBModuleWriter
{

public:

    JSBModuleWriter(JSBModule* module);

    void GenerateSource(String& sourceOut);

private:

    void WriteIncludes(String& source);
    void WriteForwardDeclarations(String& source);
    void WriteClassDeclaration(String& source);

    void WriteClassDefine(String& source);
    void WriteModulePreInit(String& source);
    void WriteModuleInit(String& source);

    JSBModule* module_;
    String source_;

};

}
