var fs = require('fs-extra');
var bcommon = require("./BuildCommon");

var buildDir = bcommon.artifactsRoot + "Build/Windows/";
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
      'cmake ../../../../ -DATOMICTOOL_NOGEN=1 -G "Visual Studio "',
      'xcodebuild -target AtomicTool -configuration Release'
    ]

    jake.exec(cmds, function() {

      var srcToolBinary = toolBuildDir + "Source/AtomicTool/Release/AtomicTool"
      fs.copySync(srcToolBinary, atomicToolBinary);
      console.log("Built MacOSX AtomicTool");
      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace


exports.clean = clean;
exports.getAtomicToolBinary = getAtomicToolBinary;
