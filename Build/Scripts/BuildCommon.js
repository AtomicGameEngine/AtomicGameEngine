var fs = require('fs-extra');
var os = require('os');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

namespace('build', function() {

  task('genscripts', {
    async: true
  }, function(platform) {

        console.log("1");
        console.log(atomicRoot);
        process.chdir(atomicRoot);

        console.log("2");
        var modules = host.getScriptModules(platform);
        var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";

        console.log("3");
        var cmds = [];
        for (var pkgName in modules) {
            cmds.push(bindCmd + "Script/Packages/" + pkgName + "/ " + platform);
            console.log(bindCmd + "Script/Packages/" + pkgName + "/ " + platform);
        }

        console.log("4");
        // Compile the Editor Scripts
        console.log("os.platform=", os.platform());
        console.log(atomicRoot + "Build/Windows/node/node.exe " + atomicRoot + "Build/TypeScript/tsc.js -p " + atomicRoot + "Script");
        if (os.platform() == "win32")
          cmds.push(atomicRoot + "Build/Windows/node/node.exe " + atomicRoot + "Build/TypeScript/tsc.js -p " + atomicRoot + "Script");
        else if (os.platform() == "darwin")
          cmds.push(atomicRoot + "Build/Mac/node/node " + atomicRoot + "Build/TypeScript/tsc.js -p " + atomicRoot + "Script");

        console.log("5");
        jake.exec(cmds, function() {
        console.log("6");

          complete();

        }, {
          printStdout: true
        });

  });


}); // end of build namespace
