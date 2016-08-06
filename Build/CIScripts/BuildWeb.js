var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var buildDir = bcommon.artifactsRoot + "Build/Web/";

namespace('build', function() {

  task('web_player', {
    async: true
  }, function() {

    var atomicTool = host.getAtomicToolBinary();

    var playerBuildDir = buildDir + "AtomicPlayer/";

    bcommon.cleanCreateDir(playerBuildDir);
    bcommon.cleanCreateDir(bcommon.getGenScriptRootDir("WEB"));

    process.chdir(playerBuildDir);

    var cmds = [

      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Atomic/ WEB",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicPlayer/ WEB",
      "cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=" + bcommon.atomicRoot + "Build/CMake/Toolchains/emscripten.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../../../",
      "make -j4"
    ]

    // TODO: Verify the follow settings for CI Build

    /*

    "cd ./Source/AtomicPlayer/Application && mv ./AtomicPlayer ./AtomicPlayer.bc && " +
    // -s ERROR_ON_UNDEFINED_SYMBOLS=1 (disabled for pthread errors currently on incoming)
    "emcc -O3 -s USE_PTHREADS=0 -s ASM_JS=1 -s VERBOSE=0 -s USE_SDL=2 -s TOTAL_MEMORY=134217728 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s NO_EXIT_RUNTIME=1 ./AtomicPlayer.bc -o  ./AtomicPlayer.html"

    */

    jake.exec(cmds, function() {

      var webPlayerBinary = playerBuildDir + "Source/AtomicPlayer/Application/AtomicPlayer.js";
      var webPlayerMemFile = playerBuildDir + "Source/AtomicPlayer/Application/AtomicPlayer.html.mem";

      fs.copySync(webPlayerBinary, buildDir + "Bin/AtomicPlayer.js");
      fs.copySync(webPlayerMemFile, buildDir + "Bin/AtomicPlayer.html.mem");

      console.log("Built Web Player");

      complete();
    }, {
      printStdout: true
    });

  });

}); // end of build namespace
