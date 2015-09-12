//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

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
