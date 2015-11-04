//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "JSBSourceWriter.h"

namespace ToolCore
{

class JSBPackage;
class JSBClass;

class JSBClassWriter : public JSBSourceWriter
{

public:

    virtual void GenerateSource(String& sourceOut);

protected:

    JSBClassWriter(JSBClass* klass);

    virtual void WriteFunctions(String& source);

    JSBClass* klass_;

};

}
