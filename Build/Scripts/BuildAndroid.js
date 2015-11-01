var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

var buildDir = host.artifactsRoot + "Build/Android/";
var editorAppFolder = host.artifactsRoot + "AtomicEditor/";

namespace('build', function() {

  task('android_player', ["build:atomiceditor"], {
    async: true
  }, function() {

    // Clean build
    common.cleanCreateDir(buildDir);

    process.chdir(buildDir);

    var cmds = [];

    var scriptModules = host.getScriptModules("ANDROID");
    var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";

    // Generate bindings for each script package
    for (var pkgName in scriptModules) {
      cmds.push(bindCmd + "Script/Packages/" + pkgName + "/ ANDROID")
    }

    // Build the Android Player
    cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAndroid.bat");

    jake.exec(cmds, function() {

      // Install Deployment
      fs.copySync(buildDir + "Source/AtomicPlayer/Application/libAtomicPlayer.so",
        editorAppFolder + "Resources/ToolData/Deployment/Android/libs/armeabi-v7a/libAtomicPlayer.so");

      complete();

    }, {
      printStdout: true,
      breakOnError : false
    });

  });

}); // end of build namespace
