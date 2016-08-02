//
// Copyright (c) 2008-2016 the Urho3D project.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

#pragma once

#include "../Core/Object.h"

namespace Atomic
{

/// Server connection established.
ATOMIC_EVENT(E_SERVERCONNECTED, ServerConnected)
{
}

/// Server connection disconnected.
ATOMIC_EVENT(E_SERVERDISCONNECTED, ServerDisconnected)
{
}

/// Server connection failed.
ATOMIC_EVENT(E_CONNECTFAILED, ConnectFailed)
{
}

/// New client connection established.
ATOMIC_EVENT(E_CLIENTCONNECTED, ClientConnected)
{
    ATOMIC_PARAM(P_CONNECTION, Connection);        // Connection pointer
}

/// Client connection disconnected.
ATOMIC_EVENT(E_CLIENTDISCONNECTED, ClientDisconnected)
{
    ATOMIC_PARAM(P_CONNECTION, Connection);        // Connection pointer
}

/// Client has sent identity: identity map is in the event data.
ATOMIC_EVENT(E_CLIENTIDENTITY, ClientIdentity)
{
    ATOMIC_PARAM(P_CONNECTION, Connection);        // Connection pointer
    ATOMIC_PARAM(P_ALLOW, Allow);                  // bool
}

/// Client has informed to have loaded the scene.
ATOMIC_EVENT(E_CLIENTSCENELOADED, ClientSceneLoaded)
{
    ATOMIC_PARAM(P_CONNECTION, Connection);        // Connection pointer
}

/// Unhandled network message received.
ATOMIC_EVENT(E_NETWORKMESSAGE, NetworkMessage)
{
    ATOMIC_PARAM(P_CONNECTION, Connection);        // Connection pointer
    ATOMIC_PARAM(P_MESSAGEID, MessageID);          // int
    ATOMIC_PARAM(P_DATA, Data);                    // Buffer
}

/// About to send network update on the client or server.
ATOMIC_EVENT(E_NETWORKUPDATE, NetworkUpdate)
{
}

/// Network update has been sent on the client or server.
ATOMIC_EVENT(E_NETWORKUPDATESENT, NetworkUpdateSent)
{
}

/// Scene load failed, either due to file not found or checksum error.
ATOMIC_EVENT(E_NETWORKSCENELOADFAILED, NetworkSceneLoadFailed)
{
    ATOMIC_PARAM(P_CONNECTION, Connection);      // Connection pointer
}

/// Remote event: adds Connection parameter to the event data
ATOMIC_EVENT(E_REMOTEEVENTDATA, RemoteEventData)
{
    ATOMIC_PARAM(P_CONNECTION, Connection);      // Connection pointer
}

// ATOMIC BEGIN

/// Master server connection ready
ATOMIC_EVENT(E_MASTERCONNECTIONREADY, MasterConnectionReady)
{
}

/// Master server connection failed
ATOMIC_EVENT(E_MASTERCONNECTIONFAILED, MasterConnectionFailed)
{
}

/// Unhandled master message received.
ATOMIC_EVENT(E_MASTERMESSAGE, MasterServerMessage)
{
    ATOMIC_PARAM(P_DATA, Data);                    // Buffer
}

// ATOMIC END

}
