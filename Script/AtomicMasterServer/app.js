const dgram = require('dgram');
const server = dgram.createSocket('udp4');
var net = require('net');
var events = require('events');
var uuid = require('uuid');
var _ = require('lodash');

var messageEventEmitter = new events.EventEmitter();

var connections = [];
var serverList = [];

var MASTER_SERVER_PORT = 41234;

// -------------------------------------------------------------------
//
// Atomic Master Server
//
//
// This server is designed to work with the MasterServerClient
// in the Atomic Game Engine to allow multiplayer servers to register
// themselves, in order to be discovered by multiplayer clients.
//
// It also handles NAT punchthrough in case the server is behind
// a NAT firewall.
//
// -------------------------------------------------------------------

// -------------------------------------------------------------------
// UDP Server
//
// We use a UDP Socket to listen for messages from both clients
// and servers. This way we can identify their external UDP ports
// assigned by any NAT firewall they might be behind.
//
// We do not send any UDP packets back. We acknowledge that we
// received the UDP port by sending a message back on the TCP socket.
//
// -------------------------------------------------------------------

function writeMessageToSocket(sock, msgObj) {
    var msg = JSON.stringify(msgObj);

    var len = msg.length;

    sock.write(len.toString());
    sock.write(':');
    sock.write(msg);
}

function handleServerUDPMessage(rinfo, msgObj) {
    console.log('Processing UDP message: ' + JSON.stringify(msgObj));

    if (msgObj.cmd === 'registerUDPPort') {
        var connectionId = msgObj.id;

        var connectionObj = _.find(connections, { connectionId: connectionId });

        if (!connectionObj) {
            console.error('No server found for id: '+connectionId);
            return;
        }

        // Save the UDP port
        connectionObj.externalUDPPort = rinfo.port;

        // Send the success message
        var udpPortMessage = {
            cmd: 'connectUDPSuccess'
        };

        writeMessageToSocket(connectionObj.tcpSocket, udpPortMessage);

        console.log('Got udp port:' + connectionObj.externalUDPPort);
    } else {
        console.log('Unable to process message: ' + msg)
    }
}

// Set up UDP
server.on('error', function(err) {
    console.log("server error: "+ err.stack);
    server.close();
});

server.on('message', function (msg, rinfo) {
    console.log('Received %d bytes from %s:%d\n', msg.length, rinfo.address, rinfo.port);

    try {
        var msgObj = JSON.parse(msg);
        handleServerUDPMessage(rinfo, msgObj);
    } catch (err) {
        console.error(err);
        console.error(err.stack);
        console.log('Unable to parse JSON from UDP: ' + message)
    }
});

server.on('listening', function () {
    var address = server.address();
    console.log("udp server listening on "+address.address + ":" + address.port);
});

server.bind(MASTER_SERVER_PORT);

// -------------------------------------------------------------------
// TCP Server
//
// We use a TCP connection to handle requests from clients and from
// servers. If the server TCP connection dies, we remove the server
// from our list.
//
// Messages are received in a simplified 'netstring' format, where
// the message length is followed by a colon, and then the actual
// message.
//
// Example: "13:Hello, World!"
//
// Messages may not be complete until multiple data calls have
// been made. Once we have a complete message, we fire the 'msg'
// event on an event emitter and handle the message.
// -------------------------------------------------------------------


function handleServerTCPMessage(socket, msgObj) {
    console.log('Processing TCP message: ' + JSON.stringify(msgObj));

    if (msgObj.cmd === 'connectRequest') {
        var connectionId = uuid.v4();

        var connectionObj = {
            connectionId: connectionId,
            internalIP: msgObj.internalIP,
            internalPort: msgObj.internalPort,
            externalIP: socket.remoteAddress,
            externalTCPPort: socket.remotePort,
            tcpSocket: socket
        };

        connections.push(connectionObj);

        // Send the uuid back to the game server
        var registerSuccessMessage = {
            cmd: 'connectTCPSuccess',
            id: connectionId
        };

        writeMessageToSocket(socket, registerSuccessMessage);

        console.log('Registered connection from IP:' + connectionObj.externalIP);
    } else if (msgObj.cmd === 'getServerList' ) {

        var servers = _.map(serverList, function (item) {
            return _.pick(item, ['connectionId', 'internalIP', 'internalPort', 'externalIP', 'externalUDPPort', 'serverName' ]);
        })

        var response = {
            cmd: 'serverList',
            servers: JSON.stringify(servers)
        }

        writeMessageToSocket(socket, response);
    } else if (msgObj.cmd === 'registerServer' ) {
        var connectionInfo = _.find(connections, { connectionId: msgObj.id });

        if (!connectionInfo) {
            console.error("No server found: " + msgObj.id);
        }

        var serverInfo = _.clone(connectionInfo);
        serverInfo.serverName = msgObj.serverName;
        serverInfo.internalIP = msgObj.internalIP;
        serverInfo.internalPort = msgObj.internalPort;

        serverList.push(serverInfo);

        console.log('Registered server: ' + serverInfo.serverName);
    } else if (msgObj.cmd === 'requestIntroduction' ) {

        var clientInfo = _.find(connections, { connectionId: msgObj.id });

        if (!clientInfo) {
            return;
            console.error("No client found: " + msgObj.id);
        }

        var serverInfo = _.find(connections, { connectionId: msgObj.serverId });

        if (!serverInfo) {
            console.error("No client found: " + msgObj.id);
            return;
        }

        // Send introduction request to server
        var response = {
            cmd: 'sendPacketToClient',
            clientId: clientInfo.connectionId,
            clientIP: clientInfo.externalIP,
            clientPort: clientInfo.externalUDPPort
        }

        // Send this to the server
        writeMessageToSocket(serverInfo.tcpSocket, response);
    } else {
        console.log('Unable to process message: ' + msg)
    }
}

messageEventEmitter.addListener('msg', function(socket, message) {
    try {
        var msgObj = JSON.parse(message);
        handleServerTCPMessage(socket, msgObj);
    } catch (err) {
        console.error(err);
        console.error(err.stack);
        console.log('Unable to parse JSON: ' + message)
    }
});

function onDisconnect(sock) {
    
}

console.log('Setting up tcp');
var tcpServer = net.createServer();
tcpServer.listen(MASTER_SERVER_PORT,'0.0.0.0');
tcpServer.on('connection', function(sock) {
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

    // Set keep alive true so we can detect when a client or server has died
    sock.setKeepAlive(true);


    var readingLength = true;
    var messageLengthStr = '';
    var bytesRemainingInMasterServerMessage = 0;
    var messageStr = '';

    // Clean up on disconnect
    sock.on('end', function() {
       
    });

    sock.on('data', function (_data) {
        var data = _data.toString();

        var n=0;
        var totalBytes = data.length;

        while (n < totalBytes) {
            var c = data[n++];

            // Are we still reading in the length?
            if (readingLength) {
                if (c == ':') {
                    bytesRemainingInMasterServerMessage = Number(messageLengthStr);
                    readingLength = false;
                }
                else {
                    messageLengthStr+=c;
                }
            }
            else {
                // Are we reading in the string?

                messageStr+=c;
                bytesRemainingInMasterServerMessage--;

                // Did we hit the end of the string?
                if (bytesRemainingInMasterServerMessage==0) {
                    messageEventEmitter.emit('msg', sock, messageStr);
                    readingLength = true;
                    messageLengthStr='';
                    messageStr='';
                }
            }
        }
    });

    sock.on('error', function(error) {
        console.log('Got TCP error: '+error);
    });

});
