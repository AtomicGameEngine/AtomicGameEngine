// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

typedef void duk_context;

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace AtomicEditor
{

class JSErrorChecker : public Object
{
    OBJECT(JSErrorChecker);

public:

    /// Construct.
    JSErrorChecker(Context* context, duk_context* ctx);
    /// Destruct.
    ~JSErrorChecker();

private:

    duk_context* ctx_;

};

}
