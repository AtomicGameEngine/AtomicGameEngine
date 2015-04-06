// https://github.com/andrewpthorp/simple-http-server

/*
// Command Line Parameters
var argv, options;
argv = require('optimist').argv;
options = {};
if (argv.p) options.port = argv.p;
if (argv.port) options.port = argv.port;
if (argv.d) options.directory = argv.d;
if (argv.directory) options.directory = argv.directory;
if (argv.nocolors) options.colors = false;
if (argv.nologs) options.nologs = true;
if (argv.help) options.help = true;
// Run the Server!
var url = require('../lib/server').run(options);
*/


/**
 * simple-http-server
 *
 * This will serve up static html, css, and javascript
 * from the current directory, or a specified directory/port
 *
 * @author Andrew Thorp
 */
(function(){

  var SWITCHES, BANNER, colors, libs, config, printLine, server;

  // colors = require('colors');

  libs = exports.libs = {
    http: require('http'),
    url: require('url'),
    fs: require('fs'),
    path: require('path')
  };

  config = exports.config = {
    port: 8000,
    directory: process.cwd(),
    colors: false,
    nologs: false
  };

  printLine = exports.printLine = function(line) {

    // Strip All Colors
    if (!config.colors){
      return process.stdout.write(line.stripColors + '\n');
    } else {
      return process.stdout.write(line + '\n');
    }

  };

  /**
   * getMimeType
   *
   * Utility function that returns the mimeType of a given file
   *
   * @author Andrew Thorp
   */
  getMimeType = exports.getMimeType = function(file){
    var i = file.lastIndexOf("."),
        ext = (i === -1) ? "default" : file.substr(i),
        mimeTypes = {
          ".bmp": "image/bmp",
          ".css": "text/css",
          ".gif": "image/gif",
          ".htm": "text/html",
          ".html": "text/html",
          ".jpg": "image/jpeg",
          ".jpeg": "image/jpeg",
          ".js": "application/javascript",
          ".json": "application/json",
          ".otf": "font/opentype",
          ".png": "image/png",
          ".text": "text/plain",
          "default": "application/octet-stream"
        };

    return mimeTypes[ext.toLowerCase()];
  }

  BANNER = 'Usage: nserver [options]\n\nIf called without options, `nserver` will listen to the current directory on port ' + config.port + '.';

  SWITCHES =  [
                ['-d', '--directory', 'root directory to listen to'],
                ['-p', '--port', 'port to listen on'],
                ['', '--launch', 'launch the server in your default browser after starting'],
                ['', '--nocolors', 'disable colors, default: false'],
                ['', '--nologs', 'disable request logs, default: false'],
                ['', '--help', 'show this help screen']
              ];

  /**
   * run
   *
   * This function is what fires off the server
   * if --help is passed as a flag, it will print the
   * help screen. If not, the server will start.
   *
   * @author Andrew Thorp
   */
  exports.run = function(options){

    // Options
    if (options !== undefined){
      if (options.help !== undefined) return usage();
      if (options.port !== undefined) exports.config.port = config.port = options.port;
      if (options.directory !== undefined) exports.config.directory = config.directory = options.directory;
      if (options.colors !== undefined) exports.config.colors = config.colors = options.colors;
      if (options.nologs !== undefined) exports.config.nologs = config.nologs = options.nologs;
    }

    // Fire off the server!
    server = libs.http.createServer(function(request, response) {

      var url = libs.url.parse(request.url, true),
          path = decodeURIComponent(url.pathname),
          result = request.method + " " + path.bold,
          fileCount = 0;

      // Default file to index.html
      if (path === "/") path += "index.html";

      var fullPath = libs.path.join(config.directory, path);

      libs.fs.exists(fullPath, function(exists){

        if (exists) {
          libs.fs.readFile(fullPath, function(error, data) {
            if (!error){
              response.writeHead(200, {
                'content-type': getMimeType(fullPath)
              });
              response.write(data);
              response.end();
              result += " - 200 OK".green;
              logString(result);
            } else {
              result += " - 500 Internal Server Error".red;
              logString(result);
            }
          });
        } else {
          response.writeHead(404, {});
          response.write("<h1>404 - Not Found</h1>");
          response.end();
          result += " - 404 Not Found".red;
          logString(result);
        }

      });

    }).listen(config.port);

    logString();

    // Return the URL
    return "http://localhost:" + config.port + "/";

  };

  /**
   * stop
   *
   * Stop the current server (if it is running)
   *
   * @author Andrew Thorp
   */
  exports.stop = function(){
    if (server){
      server.close();
    }
  }

  /**
   * usage
   *
   * print the help screen
   *
   * @author Andrew Thorp
   */
  var usage = function(){
    var lines, spaces, i, len, switchLength, currSwitch, spaces, shortFlag, longFlag;

    lines = [];
    lines.unshift("" + BANNER + "\n");

    for (i = 0, len = SWITCHES.length; i < len; i++){
      currSwitch = SWITCHES[i];
      spaces = Array(16 - currSwitch[1].length).join(' ');

      if (currSwitch[0] === ""){
        shortFlag = "    ";
      } else {
        shortFlag = currSwitch[0] + ", ";
      }

      longFlag = currSwitch[1];

      lines.push('   ' + shortFlag + longFlag + spaces + currSwitch[2]);
    }

    return printLine("\n" + (lines.join('\n')) + "\n");
  };


  /**
   * logString
   *
   * log a string (used for requests)
   *
   * @variable string (if not passed, connection string is printed)
   * @author Andrew Thorp
   */
  var logString = exports.logString = function(string){
    if (config.nologs) return;

    if (string !== undefined){
      printLine(string);
      return;
    }

    var consoleStrings, currDir;

    consoleStrings = {
      server: "simple-http-server".bold,
      directory: config.directory.green,
      shortDir: "./".green,
      url: "http://localhost:".green + config.port.toString().green + "/".green
    }

    if (__dirname.indexOf(consoleStrings.directory.stripColors) !== -1){
      currDir = consoleStrings.shortDir;
    } else {
      currDir = consoleStrings.directory;
    }

    printLine("\n" + consoleStrings.server + " Now Serving: " + currDir + " at " + consoleStrings.url + "\n");
  };

}());
