const dgram = require('dgram');
const server = dgram.createSocket('udp4');
var net = require('net');
var events = require('events');
var uuid = require('uuid');
var _ = require('lodash');

var messageEventEmitter = new events.EventEmitter();

var connections = [];
var serverList = [];

function writeMessageToSocket(sock, msgObj) {
    var msg = JSON.stringify(msgObj);

    var len = msg.length;

    console.log("Writing message to socket: "+sock+", "+msg);

    sock.write(len.toString());
    sock.write(':');
    sock.write(msg);
}

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
            return _.pick(item, ['connectionId', 'internalIP', 'internalPort', 'externalIP', 'externalUDPPort']);
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

        serverList.push(serverInfo);

        console.log('Registered server: ' + serverInfo.serverName);
    } else if (msgObj.cmd === 'requestIntroduction' ) {

    } else {
        console.log('Unable to process message: ' + msg)
    }
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

server.bind(41234);

// Set up TCP
console.log('Setting up tcp');
var tcpServer = net.createServer();
tcpServer.listen(41234);
console.log('TCP Server listening on ' + tcpServer.address().address +':'+ tcpServer.address().port);
tcpServer.on('connection', function(sock) {
    console.log('CONNECTED: ' + sock.remoteAddress +':'+ sock.remotePort);

    var readingLength = true;
    var messageLengthStr = '';
    var bytesRemainingInMasterServerMessage = 0;
    var messageStr = '';

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
});


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