common = require("./HostCommon");

// forward exports
var atomicRoot = exports.atomicRoot = common.atomicRoot;
var artifactsRoot = exports.artifactsRoot = common.artifactsRoot;
exports.cleanCreateDir = common.cleanCreateDir;

var buildDir = common.artifactsRoot + "Build/Windows/";

var atomicToolBinary = buildDir + "Source/AtomicTool/Release/AtomicTool.exe";

namespace('build', function() {

  task('atomic', {
    async: true
  }, function() {

    common.cleanCreateDir(buildDir);

    process.chdir(buildDir);

    var bindCmd = atomicToolBinary + " bind \"" + atomicRoot + "\" ";

    var cmds = [
      atomicRoot + "/Build/Scripts/Windows/CompileAtomicTool.bat",
      bindCmd + "Script/Packages/Atomic/ WINDOWS",
      bindCmd + "Script/Packages/AtomicPlayer/ WINDOWS",
      bindCmd + "Script/Packages/ToolCore/ WINDOWS",
      bindCmd + "Script/Packages/Editor/ WINDOWS",
      bindCmd + "Script/Packages/AtomicNET/ WINDOWS",
      atomicRoot + "/Build/Scripts/Windows/CompileAtomic.bat",
    ]

    jake.exec(cmds, function() {

      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
