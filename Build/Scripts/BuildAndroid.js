var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var buildDir = bcommon.artifactsRoot + "Build/Android/";

function BuildAndroidPlayer() {

  var atomicTool = host.getAtomicToolBinary();

  var playerBuildDir = buildDir + "AtomicPlayer/";

  bcommon.cleanCreateDir(playerBuildDir);

  process.chdir(playerBuildDir);

  var cmds = [
    atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Atomic/ ANDROID",
    atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicPlayer/ ANDROID"
  ];

  cmds.push("cmake -G \"Unix Makefiles\" -DCMAKE_TOOLCHAIN_FILE=" + bcommon.atomicRoot + "/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../../../");
  cmds.push("make -j4");

  jake.exec(cmds, function() {

    var androidPlayerBinary = playerBuildDir + "Source/AtomicPlayer/Application/libAtomicPlayer.so";
    fs.copySync(androidPlayerBinary, buildDir + "Bin/libAtomicPlayer.so");
    console.log("Built Android Player");
    complete();

  }, {
    printStdout: true
  });

}

namespace('build', function() {

  task('android_player', {
    async: true
  }, function() {
    BuildAndroidPlayer();
  })

}); // end of build namespace
