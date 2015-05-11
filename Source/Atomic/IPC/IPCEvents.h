#pragma once

#include "../Core/Object.h"

namespace Atomic
{

/// Worker start
EVENT(E_IPCWORKERSTART, WorkerStart)
{

}

/// Worker exited
EVENT(E_IPCWORKEREXIT, WorkerExit)
{
    PARAM(P_BROKER, Broker);   // Broker*
    PARAM(P_EXITCODE, ExitCode);   // int
}

/// Worker start
EVENT(E_IPCHELLOFROMBROKER, HelloFromBroker)
{
    PARAM(P_HELLO, Hello);      // string
    PARAM(P_LIFETHEUNIVERSEANDEVERYTHING, LifeTheUniverseAndEverything); // 42
}


}
