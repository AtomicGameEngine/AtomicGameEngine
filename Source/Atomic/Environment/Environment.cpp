
#include "Precompiled.h"
#include "Environment.h"
#include "TimeOfDay.h"
#include "ProcSky.h"

namespace Atomic
{

void RegisterEnvironmenttLibrary(Context* context)
{
    ProcSky::RegisterObject(context);
    TimeOfDay::RegisterObject(context);

}

}
