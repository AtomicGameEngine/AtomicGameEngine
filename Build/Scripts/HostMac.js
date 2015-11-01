var fs = require('fs-extra');
common = require("./HostCommon");

// forward exports
var atomicRoot = exports.atomicRoot = common.atomicRoot;
var artifactsRoot = exports.artifactsRoot = common.artifactsRoot;
exports.cleanCreateDir = common.cleanCreateDir;

var buildDir = common.artifactsRoot + "Build/Mac/";
var atomicToolBinary = buildDir + "Source/AtomicTool/Debug/AtomicTool";

namespace('build', function() {

  task('atomiceditor', {
    async: true
  }, function() {

    common.cleanCreateDir(buildDir);

    process.chdir(buildDir);

    var bindCmd = atomicToolBinary + " bind \"" + atomicRoot + "\" ";

    var cmds = [
      "cmake ../../../ -DATOMIC_DEV_BUILD=1 -G Xcode",
      "xcodebuild -target AtomicTool -configuration Debug",
      bindCmd + "Script/Packages/Atomic/ MACOSX",
      bindCmd + "Script/Packages/AtomicPlayer/ MACOSX",
      bindCmd + "Script/Packages/ToolCore/ MACOSX",
      bindCmd + "Script/Packages/Editor/ MACOSX",
      bindCmd + "Script/Packages/AtomicNET/ MACOSX",
      atomicRoot + "Build/Mac/node/node " + atomicRoot + "Build/TypeScript/tsc.js -p " + atomicRoot + "./Script",
      "cmake ../../../ -DATOMIC_DEV_BUILD=1 -G Xcode",
      "xcodebuild -configuration Debug"
    ]

    jake.exec(cmds, function() {

      fs.copySync(buildDir + "Source/AtomicEditor/Debug/AtomicEditor.app", artifactsRoot + "Bin/AtomicEditor.app");

      complete();

      console.log("AtomicEditor built to: " + artifactsRoot + "Bin/AtomicEditor.app");

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
