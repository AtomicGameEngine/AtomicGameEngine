//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/FileSystem.h>

#include "../JSBind.h"
#include "../JSBModule.h"
#include "../JSBPackage.h"
#include "../JSBEnum.h"
#include "../JSBClass.h"
#include "../JSBFunction.h"

#include "CSClassWriter.h"
#include "CSFunctionWriter.h"

namespace ToolCore
{

CSClassWriter::CSClassWriter(JSBClass *klass) : JSBClassWriter(klass)
{

}

void CSClassWriter::WriteNativeFunctions(String& source)
{
    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsDestructor())
            continue;

        CSFunctionWriter writer(function);
        writer.GenerateNativeSource(source);
    }

}

void CSClassWriter::GenerateNativeSource(String& sourceOut)
{
    String source = "";

    if (klass_->IsNumberArray())
        return;

    source.AppendWithFormat("ClassID csb_%s_GetClassID()\n{\n", klass_->GetNativeName().CString());
    source.AppendWithFormat("return %s::GetClassIDStatic();\n}\n", klass_->GetNativeName().CString());

    WriteNativeFunctions(source);

    sourceOut += source;
}

void CSClassWriter::GenerateManagedSource(String& sourceOut)
{
    String source = "";

    if (klass_->IsNumberArray())
        return;

    Indent();

    source += "\n";
    String line;

    if (klass_->GetBaseClass())
        line = "public partial class " + klass_->GetName() + " : " + klass_->GetBaseClass()->GetName() + "\n";
    else
        line = "public partial class " + klass_->GetName() + "\n";


    source += IndentLine(line);
    source += IndentLine("{\n");

    Indent();

    // managed functions
    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsDestructor())
            continue;

        CSFunctionWriter fwriter(function);
        fwriter.GenerateManagedSource(source);

    }


    Dedent();

    source += IndentLine("}\n");

    Dedent();

    sourceOut += source;
}


void CSClassWriter::GenerateSource(String& sourceOut)
{

}

}
