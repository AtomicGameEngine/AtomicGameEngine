// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "AtomicEditor.h"
#include <Atomic/Core/Context.h>
#include "JSErrorChecker.h"

namespace AtomicEditor
{

/// Construct.
JSErrorChecker::JSErrorChecker(Context* context, duk_context *ctx) :
    Object(context),
    ctx_(ctx)
{

}

/// Destruct.
JSErrorChecker::~JSErrorChecker()
{

}

}
