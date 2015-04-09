
var Q = require("q");
var fs = require("fs");
var os = require("os");
var path = require("path");
var spawn = require("child_process").spawn;

try {
  var platform_cli = require('atomic-cli-mac');
}
catch (e) {
}
try {
  var platform_cli = require('atomic-cli-windows');
}
catch (e) {
}

var mode = process.env.ATOMIC_GAME_ENGINE_ENV || 'production';
if (mode == 'dev') {
  if (process.platform == 'darwin')
    platform_cli.ATOMICTOOL_BIN = "/Users/josh/Dev/atomic/AtomicGameEngine-build/Source/AtomicTool/AtomicTool";
  else
    platform_cli.ATOMICTOOL_BIN = "C:/Dev/atomic/AtomicGameEngine-build/Source/AtomicTool/AtomicTool.exe";
}
// Commands ----------------------

var newProject = function(folder) {
  return atomictool(["new", folder], {output:true});
}

var addPlatform = function (platform) {
  return atomictool(["platform-add", platform], {output:true});
};

var build = function (platform) {

  console.log("building");
  return atomictool(["build", platform], {output:true});
};

var activate = function (productkey) {
  return atomictool(["--activate", productkey.toUpperCase().trim()], {output:true});
};

var deactivate = function () {
  return atomictool(["--deactivate"], {output:true});
};

var run = function (platform, opts) {

    opts = opts || {};
    var debug = opts.debug;

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
              open(path.resolve("Build/Mac-Build/AtomicPlayer.app"));
              break;

          case "windows":
              var open = require("open");
              open(path.resolve("Build/Windows-Build/AtomicPlayer.exe"));
              break;
        }
    };

    return opts.noBuild ? run(platform) : build([platform], opts).then(function () {
        return run();
    });
};


// Utils --------------

var exec = function (command, flags, opts) {
    opts = opts || {};
    if (opts.verbose !== false) {
        console.log([command].concat(flags).join(" "));
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


var atomictool = function (flags, opts) {
    opts = opts || {};
    flags.unshift(platform_cli.ATOMICEDITOR_DATADIR);
    flags.unshift("--cli-data-path");
    return exec(platform_cli.ATOMICTOOL_BIN, flags, opts);
};

module.exports = {
  "atomictool" : atomictool,
  "newProject" : newProject,
  "addPlatform" : addPlatform,
  "build" : build,
  "run" : run,
  "activate" : activate,
  "deactivate" : deactivate
}
