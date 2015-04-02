
var Q = require("q");
var fs = require("fs");
var ncp = require("ncp").ncp;
var os = require("os");
var path = require("path");
var spawn = require("child_process").spawn;
var wrench = require("wrench");

try {
  var platform_cli = require('atomic-cli-mac');
}
catch (e) {
}

var DATA_DIR = platform_cli.EDITOR_DATA_DIR;
var ATOMIC_TOOL_BIN = platform_cli.ATOMICTOOL_BIN;
var EDITOR_APPLICATION = platform_cli.EDITOR_APPLICATION;

var HTTP_PORT = 4000;
var SOCKET_PORT = HTTP_PORT+1;

exports.PLATFORMS = ["windows", "mac", "android", "ios", "web"];

exports.VERSION = JSON.parse(fs.readFileSync(__dirname + "/package.json")).version;

var exec = function (command, flags, opts) {
    opts = opts || {};
    if (opts.verbose !== false) {
        console.log([command].concat(flags).join(" "));
    }

    // Run everything through cmd.exe on Windows to be able to find .bat files
    if (process.platform == "win32" && opts.windowsCmd !== false) {
        flags.unshift("/c", command);
        command = "cmd";
    }

    var deferred = Q.defer();
    var child = spawn(command, flags, {stdio: (opts.output === false) ? "ignore" : "inherit"});
    child.on("close", function (code) {
        if (code && opts.check !== false) {
            deferred.reject();
        }
        deferred.resolve(code);
    });
    child.on("error", function (error) {
        deferred.reject(error);
    });
    return deferred.promise;
};
exports.exec = exec;

var atomictool = function (flags, opts) {
    opts = opts || {};
    opts.windowsCmd = false;

    flags.unshift(DATA_DIR);
    flags.unshift("--cli-data-path");
    return exec(ATOMIC_TOOL_BIN, flags, opts);
};
exports.atomictool = atomictool

var atomiceditor = function (flags, opts) {
    opts = opts || {};
    opts.detached = true;
    opts.stdio = ["ignore", "ignore", "ignore"];
    var child = spawn(EDITOR_APPLICATION, flags, opts);
    child.unref();
};
exports.atomiceditor = atomiceditor

exports.newProject = function (output) {
  return atomictool(["new", output], {output:true});
};

exports.build = function (platform) {
  return atomictool(["build", platform], {output:true});
};

exports.addPlatform = function (platform) {
  return atomictool(["platform-add", platform], {output:true});
};

exports.editor = function () {
  return atomiceditor(["-project", process.cwd()], {output:true});
};

exports.run = function (platform, opts) {
  
    opts = opts || {};
    var debug = opts.debug;

    if (platform == null) {
        // platform = get(config, "default_platform", "flash");
    }

    //checkPlatforms([platform]);

    var run = function () {
        switch (platform) {
        case "web":
            var url = "http://localhost:" + HTTP_PORT + "/AtomicPlayer.html";
            console.log("Launching: " + url);

            var open = require("open");
            open(url);
            break;

          case "mac":
              var open = require("open");
              open("Build/Mac-Build/AtomicPlayer.app");
              break;

        }
    };

    return opts.noBuild ? run() : exports.build([platform], opts).then(function () {
        console.log();
        return run();
    });
};

// Web Server (from flambe: https://raw.githubusercontent.com/aduros/flambe/master/command/index.js)
var Server = function () {
};
exports.Server = Server;

Server.prototype.start = function () {
    var self = this;
    var connect = require("connect");
    var url = require("url");
    var websocket = require("websocket");

    // Fire up a Haxe compiler server, ignoring all output. It's fine if this command fails, the
    // build will fallback to not using a compiler server
    // spawn("haxe", ["--wait", HAXE_COMPILER_PORT], {stdio: "ignore"});

    // Start a static HTTP server
    var host = "0.0.0.0";
    var staticServer = connect()
        .use(function (req, res, next) {
            var parsed = url.parse(req.url, true);
            if (parsed.pathname == "/_api") {
                // Handle API requests
                req.setEncoding("utf8");
                req.on("data", function (chunk) {
                    self._onAPIMessage(chunk)
                    .then(function (result) {
                        res.end(JSON.stringify({result: result}));
                    })
                    .catch(function (error) {
                        res.end(JSON.stringify({error: error}));
                    });
                });

            } else {
                if (parsed.query.v) {
                    // Forever-cache assets
                    var expires = new Date(Date.now() + 1000*60*60*24*365*25);
                    res.setHeader("Expires", expires.toUTCString());
                    res.setHeader("Cache-Control", "max-age=315360000");
                }
                next();
            }
        })
        .use(connect.logger("tiny"))
        .use(connect.compress())
        .use(connect.static("Build/Web-Build"))
        .listen(HTTP_PORT, host);
    console.log("Serving on http://localhost:%s", HTTP_PORT);

    this._wsServer = new websocket.server({
        httpServer: staticServer,
        autoAcceptConnections: true,
    });

    this._wsServer.on("connect", function (connection) {
        connection.on("message", function (message) {
            if (message.type == "utf8") {
                self._onMessage(message.utf8Data);
            }
        });
    });

    var net = require("net");
    this._connections = [];
    this._socketServer = net.createServer(function (connection) {
        self._connections.push(connection);
        connection.on("end", function () {
            self._connections.splice(self._connections.indexOf(connection, 1));
        });
        connection.on("data", function (data) {
            data = data.toString();
            self._onMessage(data);
        });
    });
    this._socketServer.listen(SOCKET_PORT, host);

    var watch = require("watch");
    var crypto = require("crypto");
    watch.createMonitor("Resources", {interval: 200}, function (monitor) {
        monitor.on("changed", function (file) {
            console.log("Asset changed: " + file);
            var output = "build/web/"+file;
            if (fs.existsSync(output)) {
                var contents = fs.readFileSync(file);
                fs.writeFileSync(output, contents);
                self.broadcast("file_changed", {
                    name: path.relative("Resources", file),
                    md5: crypto.createHash("md5").update(contents).digest("hex"),
                });
            }
        });
    });
};

/** Broadcast an event to all clients. */
Server.prototype.broadcast = function (type, params) {
    var event = {type: type};
    if (params) {
        for (var k in params) {
            event[k] = params[k];
        }
    }
    var message = JSON.stringify(event);
    this._wsServer.broadcast(message);
    this._connections.forEach(function (connection) {
        connection.write(message);
    });
};

/** Handle messages from connected game clients. */
Server.prototype._onMessage = function (message) {
    try {
        var event = JSON.parse(message);
        // switch (event.type) {
        // case "restart":
        //     this.broadcast("restart");
        // }
    } catch (error) {
        console.warn("Received badly formed message", error);
    }
};

/** Handle web API messages. */
Server.prototype._onAPIMessage = function (message) {
    try {
        message = JSON.parse(message);
    } catch (error) {
        return Q.reject("Badly formed JSON");
    }

    switch (message.method) {
    case "restart":
        this.broadcast("restart");
        return Q.resolve({
            htmlClients: this._wsServer.connections.length,
            flashClients: this._connections.length,
        });
    default:
        return Q.reject("Unknown method: " + message.method);
    }
};
