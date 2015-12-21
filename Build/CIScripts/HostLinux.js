var fs = require('fs-extra');
var bcommon = require("./BuildCommon");

var buildDir = bcommon.artifactsRoot + "Build/Linux/";
var atomicToolBinary = buildDir + "Bin/AtomicTool";

function clean() {

  bcommon.testRemoveDir(buildDir);

}

function getAtomicToolBinary() {

  return atomicToolBinary;

}

namespace('build', function() {

  task('atomictool', {
    async: true
  }, function() {

    var toolBuildDir = buildDir + "AtomicTool/";

    bcommon.cleanCreateDir(toolBuildDir);

    process.chdir(toolBuildDir);

    var cmds = [
        cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -DCMAKE_BUILD_TYPE=Release -DATOMIC_BUILD_2D=0 -DLINUX=1 ");
        cmds.push("make AtomicTool")
    ]

    jake.exec(cmds, function() {

      var srcToolBinary = toolBuildDir + "Source/AtomicTool/Release/AtomicTool"
      fs.copySync(srcToolBinary, atomicToolBinary);
      console.log("Built Linux AtomicTool");
      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace


exports.clean = clean;
exports.getAtomicToolBinary = getAtomicToolBinary;
