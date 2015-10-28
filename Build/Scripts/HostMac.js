var fs = require('fs-extra');
var bcommon = require("./BuildCommon");

var buildDir = bcommon.artifactsRoot + "Build/Mac/";
var atomicToolBinary = buildDir + "Bin/AtomicTool";

function clean() {

  bcommon.testRemoveDir(buildDir);

}

// Gets the path to the AtomicTool, possibly building it if it doesn't exist
function getAtomicToolBinary() {

  try {
    fs.statSync(atomicToolBinary);
    return atomicToolBinary;
  } catch (err) {
    buildAtomicTool();
    return atomicToolBinary;
  }

}

function buildAtomicTool() {

  var toolBuildDir = buildDir + "AtomicTool/";

  bcommon.cleanCreateDir(toolBuildDir);

  process.chdir(toolBuildDir);

  var cmds = [
    'cmake ../../../../ -DATOMICTOOL_NOGEN=1 -G Xcode',
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

}

exports.clean = clean;
exports.buildAtomicTool = buildAtomicTool;
exports.getAtomicToolBinary = getAtomicToolBinary;
