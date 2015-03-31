#!/usr/bin/env node
"use strict";

var argparse = require("argparse");
var fs = require("fs");
var path = require("path");
var util = require("util");
var httpreq = require("httpreq");
var cli = require("atomic-cli");

var AtomicHelpFormatter = function (opts) {
    argparse.HelpFormatter.call(this, opts);
};
util.inherits(AtomicHelpFormatter, argparse.HelpFormatter);

// http://stackoverflow.com/questions/13423540/argparse-subparser-hide-metavar-in-command-listing
AtomicHelpFormatter.prototype._formatAction = function (action) {
    var parts = argparse.HelpFormatter.prototype._formatAction.call(this, action);
    if (action.nargs == argparse.Const.PARSER) {
        var lines = parts.split("\n");
        lines.shift();
        parts = lines.join("\n");
    }
    return parts;
};

var catchErrors = function (promise) {
    promise.catch(function (error) {
        if (Array.isArray(error)) error = error[0]; // NCP throws an array of errors...?
        if (error) console.error(error.message || error);
        process.exit(1);
    });
};

var parser = new argparse.ArgumentParser({prog: "atomic-cli", formatterClass: AtomicHelpFormatter,
    description: "Atomic Game Engine CLI"});
parser.addArgument(["-v", "--version"], {action: "version", help: "Print version and exit.",
    version: cli.VERSION});

var commands = parser.addSubparsers({title: "Commands", metavar: "<command>"});

var cmd = commands.addParser("new", {help: "Create a new project.",
    description: "Creates a new project at the given path.",
    aliases: ["create"]});
cmd.addArgument(["path"], {help: "The new project directory to create."});
cmd.setDefaults({action: function (args) {
    cli.newProject(args.path)
    .then(function () {
        console.log("New Atomic project created in " + path.resolve(args.path));
    })
    .catch(function (error) {
        console.error("Error: Could not create " + path.resolve(args.path));
        process.exit(1);
    });
}});

var cmd = commands.addParser("build", {help: "Builds the project",
    description: "Builds the platform"});
cmd.addArgument(["platform"], {help: "The platform to build (windows|mac|ios|android|ios)"});
cmd.setDefaults({action: function (args) {
    cli.build(args.platform)
    .then(function () {
        console.log("Project built " + path.resolve(args.path));
    })
    .catch(function (error) {
        console.error("Error: Could not build " + path.resolve(args.path));
        process.exit(1);
    });
}});

var addCommonArguments = function (parser) {
    parser.addArgument(["--debug"], {action: "storeTrue", help: "Build in debug mode."});
};

var cmd = commands.addParser("run", {help: "Build and run on a given platform.",
    description: "Builds and runs the game on a single given platform."});
cmd.addArgument(["platform"], {metavar: "platform", nargs: "?",
    help: "A platform to target. Choose from " + cli.PLATFORMS.join(", ") + ". If omitted, 'default_platform' will be used."});
addCommonArguments(cmd);
cmd.addArgument(["--no-build"], {action: "storeTrue", help: "Don't rebuild before running."});

cmd.setDefaults({action: function (args) {
  cli.run(args.platform, {
      debug: args.debug,
      noBuild: args.no_build,
  });
}});


var cmd = commands.addParser("platform-add", {help: "Adds a platform to the project",
    description: "Adds a platform to the project"});
cmd.addArgument(["platform"], {help: "The platform to add (windows|mac|ios|android|ios)"});
cmd.setDefaults({action: function (args) {
    cli.addPlatform(args.platform)
    .then(function () {
        console.log("Platform added " + path.resolve(args.path));
    })
    .catch(function (error) {
        console.error("Error: Platform Could not be added " + path.resolve(args.path));
        process.exit(1);
    });
}});

var cmd = commands.addParser("serve", {help: "Start a development server.",
    description: "Starts a development web server for testing.",
    aliases: ["server"]});
cmd.setDefaults({action: function (args) {
            var server = new cli.Server();
            server.start();
}});

var cmd = commands.addParser("editor", {help: "Starts the Atomic Editor and loads current project",
    description: "Starts the Atomic Editor and loads current project.",
    aliases: ["edit"]});
cmd.setDefaults({action: function (args) {
            cli.editor()
}});


// GO!
if (process.argv.length > 2) {
    var args = parser.parseArgs();
    args.action(args);
} else {
    parser.printHelp();
}
