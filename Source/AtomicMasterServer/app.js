const dgram = require('dgram');
const server = dgram.createSocket('udp4');
var net = require('net');

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


function writeMessageToSocket(sock, msg) {
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
  // other stuff is the same from here

  sock.on('data', function (data) {
    console.log("tcp socket got data: "+data);
  });

  writeMessageToSocket(sock,"Hey there, how is it going?");
  writeMessageToSocket(sock,"Pretty good, how are you?");
});

