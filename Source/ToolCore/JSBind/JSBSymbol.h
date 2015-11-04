//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

namespace ToolCore
{

class JSBSymbol
{

public:

    JSBSymbol() : sourceLocation_(0), sourceLine_(0), sourceColumn_(0)
    {

    }

    String fileName_;

    unsigned sourceLocation_;

    // The line number is 1-based
    unsigned sourceLine_;
    // The column number is 1-based
    unsigned sourceColumn_;

};

}
