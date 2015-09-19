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

#include "CSBFunctionWriter.h"

namespace ToolCore
{

CSBFunctionWriter::CSBFunctionWriter(JSBFunction *function) : function_(function)
{

}

void CSBFunctionWriter::WriteParameterMarshal(String& source)
{

}


void CSBFunctionWriter::WriteConstructor(String& source)
{

}

void CSBFunctionWriter::WriteFunction(String& source)
{

}

void CSBFunctionWriter::GenerateSource(String& sourceOut)
{
    String source = "";

    if (function_->IsConstructor())
    {
        WriteConstructor(source);
    }
    else
    {
        WriteFunction(source);
    }

    sourceOut += source;

}

}
