//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#include <Atomic/IO/FileSystem.h>

#include "JSBind.h"
#include "JSBModule.h"
#include "JSBPackage.h"
#include "JSBEnum.h"
#include "JSBClass.h"
#include "JSBFunction.h"

#include "JSBClassWriter.h"
#include "JSBFunctionWriter.h"

namespace ToolCore
{

JSBClassWriter::JSBClassWriter(JSBClass *klass) : klass_(klass)
{

}


void JSBClassWriter::WriteFunctions(String& source)
{

}

void JSBClassWriter::GenerateSource(String& sourceOut)
{

}

}
