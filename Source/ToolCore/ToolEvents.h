
#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

EVENT(E_PROJECTLOADED, ProjectLoaded)
{
    PARAM(P_PROJECTPATH, ProjectPath);    // string
}

}
