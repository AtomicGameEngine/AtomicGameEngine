var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var os = require('os');
var atomicRoot = host.atomicRoot;

namespace('build', function() {

    task('atomicnet', {
        async: true
    }, function() {

        var options = host.options;

        var android = options["with-android"] ? true : false;
        var ios = options["with-ios"] ? true : false;
        var debug = options["debug"] ? true : false;

        var cmds = [];

        platforms = "-platform desktop";
        if (android)
            platforms += " -platform android";
        if (ios)
            platforms += " -platform ios";

        var netCmd = host.atomicTool + " net compile " + atomicRoot + "Script/AtomicNET/AtomicNETProject.json " + platforms + " -config " + (debug ? "Debug" : "Release");

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
