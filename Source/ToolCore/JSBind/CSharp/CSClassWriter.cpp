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

void CSClassWriter::WriteFunctions(String& source)
{
    for (unsigned i = 0; i < klass_->functions_.Size(); i++)
    {
        JSBFunction* function = klass_->functions_.At(i);

        if (function->Skip())
            continue;

        if (function->IsDestructor())
            continue;

        CSFunctionWriter writer(function);
        writer.GenerateSource(source);
    }

}

void CSClassWriter::GenerateSource(String& sourceOut)
{
    String source = "";

    if (klass_->IsNumberArray())
        return;

    WriteFunctions(source);

    sourceOut += source;

}

}
