var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

namespace('build', function() {

  task('genscripts', {
    async: true
  }, function(platform) {

        var modules = host.getScriptModules(platform);
        var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";

        var cmds = [];
        for (var pkgName in modules) {
            cmds.push(bindCmd + "Script/Packages/" + pkgName + "/ " + platform);
        }

        jake.exec(cmds, function() {

          complete();

        }, {
          printStdout: true
        });

  });


}); // end of build namespace
