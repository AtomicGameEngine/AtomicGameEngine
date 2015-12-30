var fs = require('fs-extra');
var os = require('os');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;
var glob = require('glob');

namespace('build', function() {

  task('genscripts', {
    async: true
  }, function(platform) {

        process.chdir(atomicRoot);

        var modules = host.getScriptModules(platform);
        var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";
        var node;
        var tsc = "./Build/node_modules/typeScript/lib/tsc";
        var tslint = "./Build/node_modules/tslint/lib/tslint-cli";

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
          cmds.push(node + " " + tsc + " -p ./Script");

          // lint
          // Since TSLint does not yet support recursively searching for files, then we need to
          // create a command per file.  The main issue with this is that it will abort on the first error instead
          // of listing out all lint errors
          glob("./Script/AtomicEditor/**/*.ts", (err, results) => {
            results.forEach((file)=>{
              cmds.push(node + " " + tslint + " -c ./Script/tslint.json " + file);
            });

            jake.exec(cmds, function() {

              complete();

            }, {
              printStdout: true
            });
          });
        } else {
            throw new Error("Node not configured for this platform: " + os.platform());
        }


  });


}); // end of build namespace
