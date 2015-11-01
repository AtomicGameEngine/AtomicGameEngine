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

    // Generate bindings for each script package
    var scriptPackages = ["Atomic", "AtomicPlayer"];
    var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";

    for (var i in scriptPackages) {
      cmds.push(bindCmd + "Script/Packages/" + scriptPackages[i] + "/ ANDROID")
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
