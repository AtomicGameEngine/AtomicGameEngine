// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Container/Str.h>

using namespace Atomic;

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
