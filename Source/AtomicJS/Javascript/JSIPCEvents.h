// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// Please see LICENSE.md in repository root for license information
// https://github.com/AtomicGameEngine/AtomicGameEngine

#pragma once

#include <Atomic/Core/Object.h>

namespace Atomic
{

EVENT(E_IPCJSERROR, IPCJSError)
{
    PARAM(P_ERRORNAME, ErrorName); // string
    PARAM(P_ERRORMESSAGE, ErrorMessage); // string
    PARAM(P_ERRORFILENAME, ErrorFileName); // string
    PARAM(P_ERRORLINENUMBER, ErrorLineNumber); // int
    PARAM(P_ERRORSTACK, ErrorStack); // string
}

}
