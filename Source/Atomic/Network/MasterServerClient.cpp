//
// Created by Keith Johnston on 4/16/16.
//

#include "MasterServerClient.h"
#include "../Precompiled.h"
#include "../IO/Log.h"
#include "../Network/NetworkPriority.h"
#include "../Network/Network.h"
#include "../Core/Profiler.h"
#include "../Core/CoreEvents.h"
#include "../Network/NetworkEvents.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace Atomic
{

MasterServerClient::MasterServerClient(Context *context) :
        readingMasterMessageLength(true),
        Object(context),
        udpSecondsTillRetry_(0.5f),
        connectToMasterState_(MASTER_NOT_CONNECTED),
        clientConnectToGameServerState_(GAME_NOT_CONNECTED),
        timeBetweenClientPunchThroughAttempts_(1.0),
        timeTillNextPunchThroughAttempt_(0.0),
        timeBetweenClientConnectAttempts_(1.0),
        timeTillNextClientConnectAttempt_(1.0),
        masterTCPConnection_(NULL),
        clientToServerSocket_(NULL)
{
    SubscribeToEvent(E_BEGINFRAME, ATOMIC_HANDLER(MasterServerClient, HandleBeginFrame));
}

MasterServerClient::~MasterServerClient()
{
    if (masterTCPConnection_)
    {
        masterTCPConnection_->Disconnect();
    }
}

void MasterServerClient::ConnectToMaster(const String &address, unsigned short port)
{
    ATOMIC_PROFILE(ConnectToMaster);

    if (connectToMasterState_ != MASTER_NOT_CONNECTED)
    {
        DisconnectFromMaster();
    }

    masterServerInfo_.address = address;
    masterServerInfo_.port = port;
    masterServerInfo_.isRegisteringServer = false;

    // We first make a TCP connection
    SetConnectToMasterState(MASTER_CONNECTING_TCP);
}

void MasterServerClient::DisconnectFromMaster()
{
    masterTCPConnection_->Disconnect();
    masterUDPConnection_->Disconnect();

    SetConnectToMasterState(MASTER_NOT_CONNECTED);
}

void MasterServerClient::ConnectToMasterAndRegister(const String &address, unsigned short port, const String& serverName)
{
    ATOMIC_PROFILE(ConnectToMaster);

    if (connectToMasterState_ != MASTER_NOT_CONNECTED)
    {
        DisconnectFromMaster();
    }

    masterServerInfo_.address = address;
    masterServerInfo_.port = port;
    masterServerInfo_.serverName = serverName;
    masterServerInfo_.isRegisteringServer = true;

    // We first make a TCP connection
    SetConnectToMasterState(MASTER_CONNECTING_TCP);
}

void MasterServerClient::RequestServerListFromMaster()
{
    SendMessageToMasterServer("{ \"cmd\": \"getServerList\" }");
}

void MasterServerClient::ConnectToServerViaMaster(const String &serverId,
                                                  const String &internalAddress, unsigned short internalPort,
                                                  const String &externalAddress, unsigned short externalPort,
                                                  Scene* scene)
{
    remoteGameServerInfo_.serverId = serverId;
    remoteGameServerInfo_.internalAddress = internalAddress;
    remoteGameServerInfo_.internalPort = internalPort;
    remoteGameServerInfo_.externalAddress = externalAddress;
    remoteGameServerInfo_.externalPort = externalPort;
    remoteGameServerInfo_.clientScene = scene;

    SetConnectToGameServerState(GAME_CONNECTING_INTERNAL_IP);
}

void MasterServerClient::RegisterServerWithMaster(const String &name)
{
    // Get the internal IP and port
    kNet::EndPoint localEndPoint = masterTCPConnection_->LocalEndPoint();

    unsigned char* ip = localEndPoint.ip;

    char str[256];
    sprintf(str, "%d.%d.%d.%d", (unsigned int)ip[0], (unsigned int)ip[1], (unsigned int)ip[2], (unsigned int)ip[3]);

    Atomic::Network* network = GetSubsystem<Network>();
    unsigned int localPort = network->GetServerPort();

    String msg = String("{") +
                 String("\"cmd\":") + String("\"registerServer\",") +
                 String("\"internalIP\": \"") + String(str) + String("\", ") +
                 String("\"internalPort\": ") + String(localPort) + String(", ") +
                 String("\"id\":\"") + masterServerConnectionId_ + String("\", ") +
                 String("\"serverName\":\"") + name + String("\"") +
                 String("}");

    SendMessageToMasterServer(msg);
}

void MasterServerClient::SendMessageToMasterServer(const String& msg)
{
    if (masterTCPConnection_)
    {
        String netString = String(msg.Length()) + ":" + msg;
        masterTCPConnection_->Send(netString.CString(), netString.Length());
    }
    else
    {
        ATOMIC_LOGERROR("No master server connection. Cannot send message");
    }
}

void MasterServerClient::Update(float dt)
{
    if (masterTCPConnection_==NULL)
    {
        return;
    }

    CheckForMessageFromMaster();
    ConnectToMasterUpdate(dt);
    ConnectToGameServerUpdate(dt);
    CheckForNatPunchThroughRequests(dt);
}

void MasterServerClient::CheckForNatPunchThroughRequests(float dt)
{
    if (clientIdToPunchThroughSocketMap_.Size() > 0)
    {
        if (timeTillNextPunchThroughAttempt_ <= 0)
        {
            for (HashMap<String, kNet::Socket*>::Iterator i = clientIdToPunchThroughSocketMap_.Begin();
                 i != clientIdToPunchThroughSocketMap_.End();)
            {
                Network* network = GetSubsystem<Network>();
                ATOMIC_LOGINFO("Sending packet to client");
                kNet::Socket* s = i->second_;

                if (network->IsEndPointConnected(s->RemoteEndPoint()))
                {
                    i = clientIdToPunchThroughSocketMap_.Erase(i);
                }
                else
                {
                    s->Send("K",1);
                    ++i;
                }
            }

            // Reset the timer
            timeTillNextPunchThroughAttempt_ = timeBetweenClientPunchThroughAttempts_;
        }

        timeTillNextPunchThroughAttempt_ -= dt;
    }

    // See if we need to still be trying to punch through from the client
    if (clientConnectToGameServerState_ == GAME_CONNECTING_EXTERNAL_IP &&
        clientToServerSocket_ != NULL && timeTillNextClientConnectAttempt_ <= 0)
    {
        Atomic::Network* network = GetSubsystem<Network>();

        if (!network->GetServerConnection() || !network->GetServerConnection()->IsConnected())
        {
            ATOMIC_LOGINFO("Sending packet to server");
            clientToServerSocket_->Send("K",1);
        }

        timeTillNextClientConnectAttempt_ = timeBetweenClientConnectAttempts_;
    }

    timeTillNextClientConnectAttempt_ -= dt;
}

void MasterServerClient::CheckForMessageFromMaster()
{
    kNet::OverlappedTransferBuffer *buf = masterTCPConnection_->BeginReceive();

    if (buf && buf->bytesContains > 0) {

        int n = 0;
        int totalBytes = buf->bytesContains;

        while (n < totalBytes) {
            char c = buf->buffer.buf[n++];

            // Are we still reading in the length?
            if (readingMasterMessageLength) {
                if (c == ':') {
                    sscanf(masterMessageLengthStr.CString(), "%u" , &bytesRemainingInMasterServerMessage_);
                    readingMasterMessageLength = false;

                    ATOMIC_LOGINFO("Message is " + String(bytesRemainingInMasterServerMessage_) + " long");
                }
                else {
                    masterMessageLengthStr += c;
                }
            }
            else {
                // Are we reading in the string?

                masterMessageStr += c;
                bytesRemainingInMasterServerMessage_--;

                // Did we hit the end of the string?
                if (bytesRemainingInMasterServerMessage_ == 0) {
                    HandleMasterServerMessage(masterMessageStr);
                    readingMasterMessageLength = true;
                    masterMessageLengthStr = "";
                    masterMessageStr = "";
                }
            }
        }

        masterTCPConnection_->EndReceive(buf);
    }
}

void MasterServerClient::ConnectToMasterUpdate(float dt)
{
    if (connectToMasterState_ == MASTER_NOT_CONNECTED ||
        connectToMasterState_ == MASTER_CONNECTION_FAILED)
    {
        return;
    }

    if (connectToMasterState_ == MASTER_CONNECTING_UDP)
    {
        if (udpConnectionSecondsRemaining_ <= 0)
        {
            connectToMasterState_ = MASTER_CONNECTION_FAILED;

            // TODO - emit error event

            return;
        }

        if (udpSecondsTillRetry_ <= 0)
        {
            String msg = "{ \"cmd\": \"registerUDPPort\", \"id\": \"" + masterServerConnectionId_ + "\"}";

            masterUDPConnection_->Send(msg.CString(), msg.Length());
            udpSecondsTillRetry_ = 0.5;
        }
        else
        {
            udpSecondsTillRetry_ -= dt;
            udpConnectionSecondsRemaining_ -= dt;
        }
    }

}

void MasterServerClient::HandleBeginFrame(StringHash eventType, VariantMap& eventData)
{
    using namespace BeginFrame;

    Update(eventData[P_TIMESTEP].GetFloat());
}


void MasterServerClient::SetConnectToMasterState(ConnectToMasterState state)
{
    Atomic::Network* network = GetSubsystem<Network>();

    kNet::Network* kNetNetwork = network->GetKnetNetwork();

    if (connectToMasterState_ == MASTER_NOT_CONNECTED &&
        state == MASTER_CONNECTING_TCP)
    {
        masterTCPConnection_ = kNetNetwork->ConnectSocket(masterServerInfo_.address.CString(),
                                                          masterServerInfo_.port, kNet::SocketOverTCP);

        if (!masterTCPConnection_)
        {
            SetConnectToMasterState(MASTER_CONNECTION_FAILED);
        }
        else
        {
            SendMessageToMasterServer("{ \"cmd\": \"connectRequest\" }");
        }
    }
    else if (connectToMasterState_ == MASTER_CONNECTING_TCP &&
             state == MASTER_CONNECTING_UDP)
    {
        Atomic::Network* network = GetSubsystem<Network>();
        kNet::Network* kNetNetwork = network->GetKnetNetwork();

        kNet::NetworkServer* server = kNetNetwork->GetServer().ptr();

        kNet::EndPoint masterEndPoint;

        sscanf(masterServerInfo_.address.CString(), "%hu.%hu.%hu.%hu",
               (unsigned short *) &masterEndPoint.ip[0],
               (unsigned short *) &masterEndPoint.ip[1],
               (unsigned short *) &masterEndPoint.ip[2],
               (unsigned short *) &masterEndPoint.ip[3]);

        masterEndPoint.port = masterServerInfo_.port;


        if (server)
        {
            std::vector < kNet::Socket * > listenSockets = kNetNetwork->GetServer()->ListenSockets();

            kNet::Socket *listenSocket = listenSockets[0];

            // Create a UDP and a TCP connection to the master server
            // UDP connection re-uses the same udp port we are listening on for the sever
            masterUDPConnection_ = new kNet::Socket(listenSocket->GetSocketHandle(),
                                                    listenSocket->LocalEndPoint(),
                                                    listenSocket->LocalAddress(), masterEndPoint, "",
                                                    kNet::SocketOverUDP, kNet::ServerClientSocket, 1400);
        }
        else
        {
            masterUDPConnection_ = kNetNetwork->CreateUnconnectedUDPSocket(masterServerInfo_.address.CString(),
                                                                           masterServerInfo_.port);
        }
    }

    if (state == MASTER_CONNECTION_FAILED)
    {
        ATOMIC_LOGERROR("Could not connect to master server");
        SendEvent(E_MASTERCONNECTIONFAILED);
    }

    if (state == MASTER_CONNECTED)
    {
        SendEvent(E_MASTERCONNECTIONREADY);
    }

    connectToMasterState_ = state;
}

void MasterServerClient::SetConnectToGameServerState(ClientConnectToGameServerState state)
{
    if (clientConnectToGameServerState_ == GAME_NOT_CONNECTED &&
        state == GAME_CONNECTING_INTERNAL_IP)
    {
        kNet::EndPoint serverEndPoint;
        sscanf(remoteGameServerInfo_.internalAddress.CString(), "%hu.%hu.%hu.%hu",
               (unsigned short *) &serverEndPoint.ip[0],
               (unsigned short *) &serverEndPoint.ip[1],
               (unsigned short *) &serverEndPoint.ip[2],
               (unsigned short *) &serverEndPoint.ip[3]);

        serverEndPoint.port = remoteGameServerInfo_.internalPort;

        clientToServerSocket_ = new kNet::Socket(masterUDPConnection_->GetSocketHandle(),
                                                 masterUDPConnection_->LocalEndPoint(),
                                                 masterUDPConnection_->LocalAddress(), serverEndPoint, "",
                                                 kNet::SocketOverUDP, kNet::ClientConnectionLessSocket, 1400);

        Atomic::Network* network = GetSubsystem<Network>();
        network->ConnectWithExistingSocket(clientToServerSocket_, remoteGameServerInfo_.clientScene);

        connectToGameServerSecondsRemaining_ = 5.0f;

        ATOMIC_LOGINFO("Connecting to Game Server on Internal IP: " +
                        remoteGameServerInfo_.internalAddress + ":" +
                        String(remoteGameServerInfo_.internalPort));
    }
    else if (clientConnectToGameServerState_ == GAME_CONNECTING_INTERNAL_IP &&
             state == GAME_CONNECTING_EXTERNAL_IP)
    {
        // Ask the master server to tell the game server
        // we want to connect to it.
        String msg = String("{") +
                     String("\"cmd\":") + String("\"requestIntroduction\",") +
                     String("\"id\":\"") + masterServerConnectionId_ + String("\", ") +
                     String("\"serverId\":\"") + remoteGameServerInfo_.serverId + String("\"") +
                     String("}");

        SendMessageToMasterServer(msg);

        kNet::EndPoint serverEndPoint;
        sscanf(remoteGameServerInfo_.externalAddress.CString(), "%hu.%hu.%hu.%hu",
               (unsigned short *) &serverEndPoint.ip[0],
               (unsigned short *) &serverEndPoint.ip[1],
               (unsigned short *) &serverEndPoint.ip[2],
               (unsigned short *) &serverEndPoint.ip[3]);

        serverEndPoint.port = remoteGameServerInfo_.externalPort;

        clientToServerSocket_ = new kNet::Socket(masterUDPConnection_->GetSocketHandle(),
                                                 masterUDPConnection_->LocalEndPoint(),
                                                 masterUDPConnection_->LocalAddress(), serverEndPoint, "",
                                                 kNet::SocketOverUDP, kNet::ClientConnectionLessSocket, 1400);

        Atomic::Network* network = GetSubsystem<Network>();
        network->ConnectWithExistingSocket(clientToServerSocket_, remoteGameServerInfo_.clientScene);

        connectToGameServerSecondsRemaining_ = 5.0f;

        ATOMIC_LOGINFO("Connecting to Game Server on External IP: " +
                remoteGameServerInfo_.externalAddress + ":" +
                String(remoteGameServerInfo_.externalPort));

    }

    if (state == GAME_CONNECTION_FAILED)
    {
        SendEvent(E_CONNECTFAILED);
    }

    clientConnectToGameServerState_ = state;
}

void MasterServerClient::ConnectToGameServerUpdate(float dt)
{
    if (clientConnectToGameServerState_ == GAME_NOT_CONNECTED ||
        clientConnectToGameServerState_ == GAME_CONNECTED ||
        clientConnectToGameServerState_ == GAME_CONNECTION_FAILED)
    {
       return;
    }

    Atomic::Network* network = GetSubsystem<Network>();

    // If we are connected then set the final state
    if (network->GetServerConnection() && network->GetServerConnection()->IsConnected())
    {
        if (clientConnectToGameServerState_ == GAME_CONNECTING_INTERNAL_IP)
        {
            ATOMIC_LOGINFO("Successfully connected using internal IP");
        }
        else if (clientConnectToGameServerState_ == GAME_CONNECTING_EXTERNAL_IP)
        {
            ATOMIC_LOGINFO("Successfully connected using external IP");
        }

        SetConnectToGameServerState(GAME_CONNECTED);
        return;
    }

    if (connectToGameServerSecondsRemaining_ <= 0)
    {
        if (clientConnectToGameServerState_ == GAME_CONNECTING_INTERNAL_IP)
        {
            ATOMIC_LOGINFO("Unable to connect via internal IP, trying external IP");
            SetConnectToGameServerState(GAME_CONNECTING_EXTERNAL_IP);
        }
        else
        {
            SetConnectToGameServerState(GAME_CONNECTION_FAILED);
        }
        return;
    }

    connectToGameServerSecondsRemaining_ -= dt;
}

void MasterServerClient::HandleMasterServerMessage(const String &msg)
{
    ATOMIC_LOGINFO("Got master server message: " + msg);

    rapidjson::Document document;
    if (document.Parse<0>(msg.CString()).HasParseError())
    {
        ATOMIC_LOGERROR("Could not parse JSON data from string");
        return;
    }

    String cmd = document["cmd"].GetString();

    bool sendEvent = false;

    if (cmd == "connectTCPSuccess")
    {
        udpSecondsTillRetry_ = 0;
        udpConnectionSecondsRemaining_ = 5.0;
        masterServerConnectionId_ = document["id"].GetString();

        // Now connect with UDP
        SetConnectToMasterState(MASTER_CONNECTING_UDP);
        ATOMIC_LOGINFO("TCP Connected");
    }
    else if (cmd == "connectUDPSuccess")
    {
        SetConnectToMasterState(MASTER_CONNECTED);

        // Register server if needed
        if (masterServerInfo_.isRegisteringServer)
        {
            RegisterServerWithMaster(masterServerInfo_.serverName);
        }

        ATOMIC_LOGINFO("UDP Connected");
    }
    else if (cmd == "sendPacketToClient")
    {
        String clientIP = document["clientIP"].GetString();
        int clientPort = document["clientPort"].GetInt();

        ATOMIC_LOGINFO("Got request to send packet to client at "+clientIP+":"+String(clientPort));

        kNet::EndPoint clientEndPoint;

        sscanf(clientIP.CString(), "%hu.%hu.%hu.%hu",
               (unsigned short *) &clientEndPoint.ip[0],
               (unsigned short *) &clientEndPoint.ip[1],
               (unsigned short *) &clientEndPoint.ip[2],
               (unsigned short *) &clientEndPoint.ip[3]);


        clientEndPoint.port = clientPort;

        // Create a socket that goes out the same port we are listening on to the client.
        // This will be used until the client actually connects.
        kNet::Socket* s = new kNet::Socket(masterUDPConnection_->GetSocketHandle(),
                                           masterUDPConnection_->LocalEndPoint(),
                                           masterUDPConnection_->LocalAddress(), clientEndPoint, "",
                                           kNet::SocketOverUDP, kNet::ServerClientSocket, 1400);

        String clientId = document["clientId"].GetString();
        clientIdToPunchThroughSocketMap_.Insert(MakePair(clientId, s));
    }
    else if (cmd == "serverList")
    {
        sendEvent = true;
    }

    if (sendEvent)
    {
        using namespace NetworkMessage;

        VariantMap& eventData = GetEventDataMap();
        eventData[P_DATA] = msg;
        SendEvent(E_MASTERMESSAGE, eventData);
    }
}


bool MasterServerClient::StartServerAndRegisterWithMaster(unsigned short serverPort, const String &masterAddress,
                                                          unsigned short masterPort, const String &serverName)
{
    Network* network = GetSubsystem<Network>();

    if (!network)
    {
        ATOMIC_LOGERROR("MasterServerClient::StartServerAndRegisterWithMaster - Unable to get Network subsystem");
        return false;
    }

    // First start the server
    bool rc = network->StartServer(serverPort);

    if (!rc)
    {
        ATOMIC_LOGERROR("MasterServerClient::StartServerAndRegisterWithMaster - Unable to start server");
        return false;
    }

    // Connect to the master server
    ConnectToMasterAndRegister(masterAddress, masterPort, serverName);

    return true;
}

}
