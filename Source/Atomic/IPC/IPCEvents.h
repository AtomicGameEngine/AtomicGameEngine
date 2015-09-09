#pragma once

#include "../Core/Object.h"

namespace Atomic
{

/// Worker start
EVENT(E_IPCWORKERSTART, IPCWorkerStart)
{

}

/// Worker exited
EVENT(E_IPCWORKEREXIT, IPCWorkerExit)
{
    PARAM(P_BROKER, Broker);   // Broker*
    PARAM(P_EXITCODE, ExitCode);   // int
}

/// broker -> worker start
EVENT(E_IPCINITIALIZE, IPCInitialize)
{

}

/// Worker Log
EVENT(E_IPCWORKERLOG, IPCWorkerLog)
{
    PARAM(P_LEVEL, Level);      // int log level
    PARAM(P_MESSAGE, Message);  // string
}

/// Worker Log
EVENT(E_IPCMESSAGE, IPCMessage)
{
    PARAM(P_MESSAGE, Message);  // string
    PARAM(P_VALUE, Value);  // int
}


}
