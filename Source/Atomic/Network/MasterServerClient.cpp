//
// Created by Keith Johnston on 4/16/16.
//

#include "MasterServerClient.h"
#include "../Precompiled.h"
#include <vector>
#include <inttypes.h>
#include "../IO/Log.h"
#include "../Network/NetworkPriority.h"
#include "../Network/Network.h"
#include "../Core/Profiler.h"

#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>

namespace Atomic
{

MasterServerClient::MasterServerClient(Context *context) :
        readingMasterMessageLength(true),
        Object(context),
        udpTimeout_(5.0f),
        udpSecondsTillRetry_(0.5f),
        isTCPConnected(false),
        isUDPConnected(false)
{

}

MasterServerClient::~MasterServerClient()
{

}

bool MasterServerClient::ConnectToMaster(const String &address, unsigned short port) {
    PROFILE(ConnectToMaster);

    sscanf(address.CString(), "%hu.%hu.%hu.%hu",
           (unsigned short *) &masterEndPoint_.ip[0],
           (unsigned short *) &masterEndPoint_.ip[1],
           (unsigned short *) &masterEndPoint_.ip[2],
           (unsigned short *) &masterEndPoint_.ip[3]);

    masterEndPoint_.port = port;

    Atomic::Network* network = GetSubsystem<Network>();
    kNet::Network* kNetNetwork = network->GetKnetNetwork();

    std::vector < kNet::Socket * > listenSockets = kNetNetwork->GetServer()->ListenSockets();

    kNet::Socket *listenSocket = listenSockets[0];

    // Create a UDP and a TCP connection to the master server
    // UDP connection re-uses the same udp port we are listening on for the sever
    masterUDPConnection_ = new kNet::Socket(listenSocket->GetSocketHandle(),
                                            listenSocket->LocalEndPoint(),
                                            listenSocket->LocalAddress(), masterEndPoint_, "",
                                            kNet::SocketOverUDP, kNet::ServerClientSocket, 1400);

    masterTCPConnection_ = kNetNetwork->ConnectSocket(address.CString(), port, kNet::SocketOverTCP);

    SendMessageToMasterServer("{ \"cmd\": \"connectRequest\" }");

    return true;
}

void MasterServerClient::SendMessageToMasterServer(const String& msg)
{
    String netString = String(msg.Length()) + ':' + msg;
    masterTCPConnection_->Send(netString.CString(), netString.Length());
}

void MasterServerClient::Update(float dt) {
    // Check for messages from master
    if (masterTCPConnection_) {
        kNet::OverlappedTransferBuffer *buf = masterTCPConnection_->BeginReceive();

        if (buf && buf->bytesContains > 0) {

            int n = 0;
            int totalBytes = buf->bytesContains;

            while (n < totalBytes) {
                char c = buf->buffer.buf[n++];

                // Are we still reading in the length?
                if (readingMasterMessageLength) {
                    if (c == ':') {
                        sscanf(masterMessageLengthStr.CString(), "%" SCNd32, &bytesRemainingInMasterServerMessage_);
                        readingMasterMessageLength = false;

                        LOGINFO("Message is "+String(bytesRemainingInMasterServerMessage_)+" long");
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

    if (isConnectingUDP_)
    {
        ConnectUDP(dt);
    }
}

void MasterServerClient::ConnectUDP(float dt)
{
    if (udpConnectionSecondsRemaining_ <= 0)
    {
        isConnectingUDP_ = false;

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

void MasterServerClient::HandleMasterServerMessage(const String &msg)
{
    LOGINFO("Got master server message: " + msg);

    rapidjson::Document document;
    if (document.Parse<0>(msg.CString()).HasParseError())
    {
        LOGERROR("Could not parse JSON data from string");
        return;
    }

    String cmd = document["cmd"].GetString();

    if (cmd == "connectTCPSuccess")
    {
        isTCPConnected = true;
        isConnectingUDP_ = true;
        udpSecondsTillRetry_ = 0;
        udpConnectionSecondsRemaining_ = 5.0;
        masterServerConnectionId_ = document["id"].GetString();

        LOGINFO("TCP Connected");
    }
    if (cmd == "connectUDPSuccess")
    {
        isUDPConnected = true;
        isConnectingUDP_ = false;

        LOGINFO("UDP Connected");

        // TODO - emit success event
    }
}


}