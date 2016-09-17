var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var os = require('os');
var config = require('./BuildConfig');

var atomicRoot = config.atomicRoot;

namespace('build', function() {

    task('atomicnet', {
        async: true
    }, function() {


        platforms = "-platform desktop";

        if (config["with-android"])
            platforms += " -platform android";

        if (config["with-ios"])
            platforms += " -platform ios";

        var cmds = [];

        var netCmd = host.atomicTool + " net compile " + atomicRoot + "Script/AtomicNET/AtomicNETProject.json " + platforms + " -config " + config["config"];

        console.log(netCmd);

        cmds.push(netCmd);

        jake.exec(cmds, function() {

            complete();

        }, {
            printStdout: true,
            printStderr: true
        });

    });

}); // end of build namespace
