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
        var node;

        switch(os.platform()) {
            case "win32":
                node = "Build/Windows/node/node.exe";
                break;
            case "darwin":
                node = "Build/Mac/node/node";
                break;
            case "linux":
                node = "Build/Linux/node/node";
                break;
        }

        var cmds = [];
        for (var pkgName in modules) {
            cmds.push(bindCmd + "Script/Packages/" + pkgName + "/ " + platform);
        }

        if (node) {
          // compile
          cmds.push(node + " ./Build/node_modules/typeScript/bin/tsc -p ./Script");
          // lint
          cmds.push(node + " ./Build/node_modules/tslint/bin/tslint -c ./Script/tslint.json ./Script/TypeScript/*.ts");
          cmds.push(node + " ./Build/node_modules/tslint/bin/tslint -c ./Script/tslint.json ./Script/AtomicEditor/**/*.ts");
        } else {
            throw new Error("Node not configured for this platform: " + os.platform());
        }

         jake.exec(cmds, function() {

          complete();

        }, {
          printStdout: true
        });

  });


}); // end of build namespace
