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

enum ConnectToMasterState
{
    MASTER_NOT_CONNECTED = 0,
    MASTER_CONNECTING_UDP,
    MASTER_CONNECTING_TCP,
    MASTER_CONNECTED,
    MASTER_CONNECTION_FAILED
};

enum ClientConnectToGameServerState
{
    GAME_NOT_CONNECTED = 0,
    GAME_CONNECTING_INTERNAL_IP,
    GAME_VERIFYING_SERVER,
    GAME_CONNECTING_EXTERNAL_IP,
    GAME_CONNECTED,
    GAME_CONNECTION_FAILED
};

struct MasterServer
{
    String address;
    unsigned short port;
    bool isRegisteringServer;
    String serverName;
};

/// Game server info used by a client when connecting
struct RemoteGameServer
{
    /// Game server id
    String serverId;
    String internalAddress;
    unsigned short internalPort;
    String externalAddress;
    unsigned short externalPort;
    Scene* clientScene;
};


/// Client for master server, see Script/AtomicMasterServer for example master server implementation
class ATOMIC_API MasterServerClient : public Object
{
    ATOMIC_OBJECT(MasterServerClient, Object);

public:

    /// Construct.
    MasterServerClient(Context* context);
    /// Destruct.
    ~MasterServerClient();

    void ConnectToMaster(const String& address, unsigned short port);

    void DisconnectFromMaster();

    void RequestServerListFromMaster();

    void ConnectToServerViaMaster(const String& serverId,
                                  const String& internalAddress, unsigned short internalPort,
                                  const String& externalAddress, unsigned short externalPort,
                                  Scene* scene);

    void ConnectToMasterAndRegister(const String& address, unsigned short port, const String& serverName);

    /// Convenience method to start server and register it with the master server
    bool StartServerAndRegisterWithMaster(unsigned short serverPort, const String &masterAddress,
                                          unsigned short masterPort, const String &serverName);

private:

    void HandleBeginFrame(StringHash eventType, VariantMap& eventData);

    void SendMessageToMasterServer(const String& message);

    void HandleMasterServerMessage(const String& msg);

    void RegisterServerWithMaster(const String& serverName);

    /// Process messages
    void Update(float timeStep);

    float udpConnectionSecondsRemaining_;
    float udpSecondsTillRetry_;

    // Communication with Master Server
    bool readingMasterMessageLength;
    uint32_t bytesRemainingInMasterServerMessage_;
    String masterMessageLengthStr;
    String masterMessageStr;
    kNet::Socket* masterUDPConnection_;
    kNet::Socket* masterTCPConnection_;

    String masterServerConnectionId_;

    void SetConnectToGameServerState(ClientConnectToGameServerState state);
    void SetConnectToMasterState(ConnectToMasterState state);

    void ConnectToMasterUpdate(float dt);
    void ConnectToGameServerUpdate(float dt);

    float timeBetweenClientPunchThroughAttempts_;
    float timeTillNextPunchThroughAttempt_;

    float timeBetweenClientConnectAttempts_;
    float timeTillNextClientConnectAttempt_;

    HashMap<String, kNet::Socket*> clientIdToPunchThroughSocketMap_;
    kNet::Socket* clientToServerSocket_;

    ConnectToMasterState connectToMasterState_;
    ClientConnectToGameServerState clientConnectToGameServerState_;

    RemoteGameServer remoteGameServerInfo_;
    float connectToGameServerSecondsRemaining_;

    MasterServer masterServerInfo_;

    void CheckForMessageFromMaster();

    void CheckForNatPunchThroughRequests(float dt);
};

}
