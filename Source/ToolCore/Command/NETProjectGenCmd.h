//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include "Command.h"

using namespace Atomic;

namespace ToolCore
{

class NETProjectGenCmd: public Command
{
    OBJECT(NETProjectGenCmd);

public:

    NETProjectGenCmd(Context* context);
    virtual ~NETProjectGenCmd();

    bool Parse(const Vector<String>& arguments, unsigned startIndex, String& errorMsg);

    void Run();

    bool RequiresProjectLoad() { return false; }

private:

    String scriptPlatform_;
    String projectFile_;

};

}
