var fs = require('fs-extra');
var os = require('os');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

namespace('build', function() {

  task('genscripts', {
    async: true
  }, function(platform) {

        process.chdir(atomicRoot);

        var modules = host.getScriptModules(platform);
        var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";

        var cmds = [];
        for (var pkgName in modules) {
            cmds.push(bindCmd + "Script/Packages/" + pkgName + "/ " + platform);
        }

        // Compile the Editor Scripts
        if (os.platform() == "win32")
          cmds.push(atomicRoot + "Build/Windows/node/node.exe " + atomicRoot + "Build/TypeScript/tsc.js -p " + atomicRoot + "Script");
        else if (os.platform() == "darwin")
          cmds.push(atomicRoot + "Build/Mac/node/node " + atomicRoot + "Build/TypeScript/tsc.js -p " + atomicRoot + "Script");
        else if (os.platform() == "linux") {
          cmds.push(atomicRoot + "Build/Linux/node/node " + atomicRoot + "Build/TypeScript/tsc.js -p " + atomicRoot + "Script");
        }
        jake.exec(cmds, function() {

          complete();

        }, {
          printStdout: true
        });

  });


}); // end of build namespace
