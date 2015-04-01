// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

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


// license error
EVENT(E_LICENSE_ERROR, LicenseError)
{

}

}
