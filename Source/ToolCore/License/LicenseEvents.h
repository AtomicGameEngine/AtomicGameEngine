//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

#pragma once

#include <Atomic/Core/Object.h>

using namespace Atomic;

namespace ToolCore
{

// requires EULA agreement
EVENT(E_LICENSE_EULAREQUIRED, LicenseEulaRequired)
{

}

// activation required
EVENT(E_LICENSE_ACTIVATIONREQUIRED, LicenseActivationRequired)
{

}

// license verification required
EVENT(E_LICENSE_VERIFICATIONREQUIRED, LicenseVerificationRequired)
{

}

// license success
EVENT(E_LICENSE_SUCCESS, LicenseSuccess)
{

}

EVENT(E_LICENSE_ACTIVATIONSUCCESS, LicenseActivationSuccess)
{
}

EVENT(E_LICENSE_ACTIVATIONERROR, LicenseActivationError)
{
    PARAM(P_MESSAGE, Message); // String
}

EVENT(E_LICENSE_DEACTIVATIONSUCCESS, LicenseDeactivationSuccess)
{
}

EVENT(E_LICENSE_DEACTIVATIONERROR, LicenseDeactivationError)
{
    PARAM(P_MESSAGE, Message); // String
}


// license error
EVENT(E_LICENSE_ERROR, LicenseError)
{

}

}
