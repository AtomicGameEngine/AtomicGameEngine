// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#include "Precompiled.h"
#include "Environment.h"
#include "TimeOfDay.h"
#include "ProcSky.h"

namespace Atomic
{

void RegisterEnvironmentLibrary(Context* context)
{
    ProcSky::RegisterObject(context);
    TimeOfDay::RegisterObject(context);

}

}
