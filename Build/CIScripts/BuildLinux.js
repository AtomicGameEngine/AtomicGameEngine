var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var buildDir = bcommon.artifactsRoot + "Build/Linux/";

namespace('build', function() {

  task('linux_player', {
    async: true
  }, function() {

    var atomicTool = host.getAtomicToolBinary();

    var playerBuildDir = buildDir + "AtomicPlayer/";

    bcommon.cleanCreateDir(playerBuildDir);
    bcommon.cleanCreateDir(bcommon.getGenScriptRootDir("LINUX"));

    process.chdir(playerBuildDir);

    var cmds = [
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Atomic/ LINUX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicPlayer/ LINUX",
      "cmake -DATOMIC_DEV_BUILD=0 -DLINUX=1 ../../../../",
      "make"
    ];

    jake.exec(cmds, function() {
      var macPlayerBinary = playerBuildDir + "Source/AtomicPlayer/Application/Release/Contents/Linux/AtomicPlayer";
      fs.copySync(macPlayerBinary, buildDir + "Bin/AtomicPlayer");
      console.log("Built Linux Player");
      complete();

    }, {
      printStdout: true
    });

  });

  task('linux_editor', {
    async: true
  }, function() {

    var atomicTool = host.getAtomicToolBinary();

    var editorBuildDir = buildDir + "AtomicEditor/";

    bcommon.cleanCreateDir(editorBuildDir);

    process.chdir(editorBuildDir);

    var cmds = [
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Atomic/ LINUX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicPlayer/ LINUX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/ToolCore/ LINUX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Editor/ LINUX",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/WebView/ LINUX",
      "cmake -DATOMIC_DEV_BUILD=0 -DLINUX=1 ../../../../",
      "make -j4"
    ];

    jake.exec(cmds, function() {
      var macEditorBinary = editorBuildDir + "Source/AtomicEditor/Release/Contents/Linux/AtomicEditor";
      fs.copySync(macEditorBinary, buildDir + "Bin/AtomicEditor");
      console.log("Built Linux Editor");
      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
