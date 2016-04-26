var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var os = require('os');
var atomicRoot = host.atomicRoot;

var buildDir = host.artifactsRoot + "Build/Web/AtomicPlayer";

// build command:
// ./Build/Mac/node/node ./Build/node_modules/jake/bin/cli.js -f ./Build/Scripts/Bootstrap.js build:web_player

namespace('build', function() {

  task('web_player'/*, ["build:atomiceditor"]*/, {
    async: true
  }, function() {

    // TODO: add clean build option
    if (!fs.existsSync(buildDir)) {
      common.cleanCreateDir(buildDir);
    }

    process.chdir(buildDir);

    var cmds = [];

    var scriptModules = host.getScriptModules("WEB");

    var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";

    // Generate bindings for each script package
    for (var pkgName in scriptModules) {
      cmds.push(bindCmd + "Script/Packages/" + pkgName + "/ WEB")
    }

    if (os.platform() == "win32") {

    }
    else {
      cmds.push("cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=\"" + atomicRoot + "Build/CMake/Toolchains/emscripten.toolchain.cmake\" -DCMAKE_BUILD_TYPE=Release ../../../../");
      cmds.push("make -j4");
      // -s ERROR_ON_UNDEFINED_SYMBOLS=1 (disabled for pthread errors currently on incoming)
      cmds.push("cd ./Source/AtomicPlayer/Application && mv ./AtomicPlayer ./AtomicPlayer.bc && " +
      "emcc -O3 -s USE_PTHREADS=0 -s ASM_JS=1 -s VERBOSE=0 -s USE_SDL=2 -s TOTAL_MEMORY=134217728 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s NO_EXIT_RUNTIME=1 ./AtomicPlayer.bc -o ./AtomicPlayer.html");
    }

    jake.exec(cmds, function() {

      var editorAppFolder = host.artifactsRoot + (os.platform() == "win32" ? "AtomicEditor/" : "AtomicEditor/AtomicEditor.app/");

      var webPlayerBinary = buildDir + "/Source/AtomicPlayer/Application/AtomicPlayer.js";
      var webPlayerMemFile = buildDir + "/Source/AtomicPlayer/Application/AtomicPlayer.html.mem";

      // Install Deployment

      // Don't put into Artifact editor build as this requires a Emscripten install
      // fs.copySync(webPlayerBinary, editorAppFolder + "Resources/ToolData/Deployment/Web/AtomicPlayer.js");
      // fs.copySync(webPlayerMemFile, editorAppFolder + "Resources/ToolData/Deployment/Web/AtomicPlayer.html.mem");

      // Instead, use local build
      fs.copySync(webPlayerBinary, atomicRoot + "Data/AtomicEditor/Deployment/Web/AtomicPlayer.js");
      fs.copySync(webPlayerMemFile, atomicRoot + "Data/AtomicEditor/Deployment/Web/AtomicPlayer.html.mem");

      console.log("Built Web Player");

      complete();

    }, {
      printStdout: true,
      breakOnError : false
    });

  });

}); // end of build namespace
