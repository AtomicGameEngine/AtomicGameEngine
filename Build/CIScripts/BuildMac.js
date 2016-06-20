var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var buildDir = bcommon.artifactsRoot + "Build/Mac/";

namespace('build', function() {

  task('mac_player', {
    async: true
  }, function() {

    var atomicTool = host.getAtomicToolBinary();

    var playerBuildDir = buildDir + "AtomicPlayer/";

    bcommon.cleanCreateDir(playerBuildDir);
    bcommon.cleanCreateDir(bcommon.getGenScriptRootDir("MACOSX"));

    process.chdir(playerBuildDir);

    var cmds = [
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Atomic/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicPlayer/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicApp/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicNETNative/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicNETScript/ MACOSX",
      "cmake -DATOMIC_DEV_BUILD=0 -G Xcode ../../../../",
      "xcodebuild -target AtomicPlayer -configuration Release -parallelizeTargets -jobs 4"
    ];

    jake.exec(cmds, function() {
      var macPlayerBinary = playerBuildDir + "Source/AtomicPlayer/Application/Release/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";
      fs.copySync(macPlayerBinary, buildDir + "Bin/AtomicPlayer");
      console.log("Built Mac Player");
      complete();

    }, {
      printStdout: true
    });

  });

  task('mac_editor', {
    async: true
  }, function() {

    var atomicTool = host.getAtomicToolBinary();

    var editorBuildDir = buildDir + "AtomicEditor/";

    bcommon.cleanCreateDir(editorBuildDir);

    process.chdir(editorBuildDir);

    var cmds = [
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Atomic/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicPlayer/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/ToolCore/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Editor/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/WebView/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicApp/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicNETNative/ MACOSX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicNETScript/ MACOSX",      
      "cmake -DATOMIC_DEV_BUILD=0 -G Xcode ../../../../",
      "xcodebuild -target AtomicEditor -configuration Release -parallelizeTargets -jobs 4",
      "cd " + editorBuildDir + "Source/AtomicEditor/Release && zip -r -X " + buildDir + "Bin/AtomicEditor.zip ./AtomicEditor.app"
    ];

    jake.exec(cmds, function() {

      console.log("Built Mac Editor");
      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
