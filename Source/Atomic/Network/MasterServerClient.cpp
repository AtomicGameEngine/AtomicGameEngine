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

namespace Atomic
{

MasterServerClient::MasterServerClient(Context *context) :
        readingMasterMessageLength(true),
        Object(context)
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

    String msg = "{\"cmd\":\"registerGameServer\"}";
    String netString = String(msg.Length()) + ':' + msg;
    masterTCPConnection_->Send(netString.CString(), netString.Length());

    return true;
}

void MasterServerClient::Update(float dt) {
    // Check for messages from master
    if (masterTCPConnection_) {
        kNet::OverlappedTransferBuffer *buf = masterTCPConnection_->BeginReceive();

        if (buf && buf->bytesContains > 0) {
            String tmp(buf->buffer.buf);

            int n = 0;
            int totalBytes = tmp.Length();

            while (n < totalBytes) {
                char c = tmp[n++];

                // Are we still reading in the length?
                if (readingMasterMessageLength) {
                    if (c == ':') {
                        sscanf(masterMessageLengthStr.CString(), "%" SCNd32, &bytesRemainingInMasterServerMessage_);
                        readingMasterMessageLength = false;
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
}

void MasterServerClient::HandleMasterServerMessage(const String &msg) {
    LOGINFO("Got master server message: " + msg);
}


}