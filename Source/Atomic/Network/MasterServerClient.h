//
// Communication with an atomic master server
// Handles NAT Punch-through for clients
//

#pragma once

#include <ThirdParty/kNet/include/kNet.h>
#include "../Core/Object.h"

namespace Atomic
{

/// %MasterServerClient subsystem.
class ATOMIC_API MasterServerClient : public Object
{
    OBJECT(MasterServerClient);

public:
    /// Construct.
    MasterServerClient(Context* context);
    /// Destruct.
    ~MasterServerClient();

    /// Process messages
    void Update(float timeStep);

    bool ConnectToMaster(const String& address, unsigned short port);
    void SendMessageToMasterServer(const String& message);

private:
    void HandleMasterServerMessage(const String& msg);

    bool isConnectingUDP_;
    float udpTimeout_;
    float udpConnectionSecondsRemaining_;
    float udpSecondsTillRetry_;

    bool readingMasterMessageLength;
    uint32_t bytesRemainingInMasterServerMessage_;
    String masterMessageLengthStr;
    String masterMessageStr;
    kNet::EndPoint masterEndPoint_;
    kNet::Socket* masterUDPConnection_;
    kNet::Socket* masterTCPConnection_;

    bool isTCPConnected;
    bool isUDPConnected;

    String masterServerConnectionId_;

    void ConnectUDP(float dt);
};

}