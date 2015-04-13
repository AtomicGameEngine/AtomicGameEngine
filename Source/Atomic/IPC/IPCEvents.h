#pragma once

#include "../Core/Object.h"

namespace Atomic
{

/// Worker start
EVENT(E_IPCWORKERSTART, WorkerStart)
{

}

/// Worker start
EVENT(E_IPCHELLOFROMBROKER, HelloFromBroker)
{
    PARAM(P_HELLO, Hello);      // string
    PARAM(P_LIFETHEUNIVERSEANDEVERYTHING, LifeTheUniverseAndEverything); // 42
}


}
