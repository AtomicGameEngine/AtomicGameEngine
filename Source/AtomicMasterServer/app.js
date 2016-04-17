const dgram = require('dgram');
const server = dgram.createSocket('udp4');
var net = require('net');
var events = require('events');
var uuid = require('uuid');
var _ = require('lodash');

var messageEventEmitter = new events.EventEmitter();

var registeredServers = [];

// Set up UDP
server.on('error', function(err) {
    console.log("server error: "+ err.stack);
    server.close();
});

server.on('message', function (msg, rinfo) {
    console.log('Received %d bytes from %s:%d\n', msg.length, rinfo.address, rinfo.port);

    console.log(msg);
});

server.on('listening', function () {
    var address = server.address();
    console.log("udp server listening on "+address.address + ":" + address.port);
});

server.bind(41234);


function writeMessageToSocket(sock, msgObj) {
    var msg = JSON.stringify(msgObj);
    
    var len = msg.length;

    sock.write(len.toString());
    sock.write(':');
    sock.write(msg);
}


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

function handleServerMessage(socket, msgObj) {
    console.log('Processing message: ' + JSON.stringify(msgObj));

    if (msgObj.cmd === 'registerGameServer') {
        var serverId = uuid.v4();

        var gameServerObj = {
            serverId: serverId,
            internalIP: msgObj.internalIP,
            internalPort: msgObj.internalPort,
            externalIP: socket.remoteAddress,
            externalTCPPort: socket.remotePort
        };

        registeredServers.push(gameServerObj);

        // Send the uuid back to the game server
        var registerSuccessMessage = {
            cmd: 'registerSuccess',
            id: serverId
        };

        writeMessageToSocket(socket, registerSuccessMessage);

        console.log('Registered game server on IP:' + gameServerObj.externalIP);
    } else {
        console.log('Unable to process message: ' + msg)
    }
}

messageEventEmitter.addListener('msg', function(socket, message) {
    try {
        var msgObj = JSON.parse(message);
        handleServerMessage(socket, msgObj);
    } catch (err) {
        console.error(err);
        console.error(err.stack);
        console.log('Unable to parse JSON: ' + message)
    }
});