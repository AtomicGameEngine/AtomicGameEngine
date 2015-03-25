
var Q = require("q");
var fs = require("fs");
var ncp = require("ncp").ncp;
var os = require("os");
var path = require("path");
var spawn = require("child_process").spawn;
var wrench = require("wrench");

var DATA_DIR = __dirname + "/data/";
exports.DATA_DIR = DATA_DIR;

var ATOMIC_TOOL_BIN = __dirname + "/data/bin/Mac/AtomicTool";

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

exports.newProject = function (output) {
  return atomictool(["new", output], {output:true});
};;
