//
// Communication with an atomic master server
// Handles NAT Punch-through for clients
//

#pragma once

#include <ThirdParty/kNet/include/kNet.h>
#include "../Core/Object.h"

namespace Atomic
{

class Scene;

class ATOMIC_API MasterServerEntry : public Object
{
    OBJECT(MasterServerEntry);

public:
    MasterServerEntry(Context* context);

    String serverId;
};

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

    void ConnectToMaster(const String& address, unsigned short port);
    void RequestServerListFromMaster();
    void RegisterServerWithMaster(const String& serverName);
    void ConnectToServerViaMaster(const String& serverId, const String& address, unsigned short port, Scene* scene);

private:
    void SendMessageToMasterServer(const String& message);
    void HandleMasterServerMessage(const String& msg);

    // Used by the client
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

    float timeBetweenClientPunchThroughAttempts_;
    float timeTillNextPunchThroughAttempt_;

    float timeBetweenClientConnectAttempts_;
    float timeTillNextClientConnectAttempt_;

    HashMap<String, kNet::Socket*> clientIdToPunchThroughSocketMap_;
    kNet::Socket* clientToServerSocket_;
    Scene* clientPendingScene_;
};

}