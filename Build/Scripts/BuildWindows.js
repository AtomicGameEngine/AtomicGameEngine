var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

var buildDir = host.artifactsRoot + "Build/Windows/";
var editorAppFolder = host.artifactsRoot + "AtomicEditor/";

namespace('build', function() {

  task('atomiceditor', {
    async: true
  }, function() {

    // Clean build
    common.cleanCreateDir(buildDir);
    common.cleanCreateDir(editorAppFolder);

    process.chdir(buildDir);

    var cmds = [];

    // Build the AtomicTool
    cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAtomicTool.bat");

    // Generate bindings for each script package

    var scriptPackages = host.getScriptPackages();
    var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";

    for (var i in scriptPackages) {
      cmds.push(bindCmd + "Script/Packages/" + scriptPackages[i] + "/ WINDOWS")
    }

    // Build the AtomicEditor
    cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAtomicEditor.bat");

    jake.exec(cmds, function() {

      // Copy the Editor binaries
      fs.copySync(buildDir + "Source/AtomicEditor/Release/AtomicEditor.exe",
        host.artifactsRoot + "AtomicEditor/AtomicEditor.exe");
      fs.copySync(buildDir + "Source/AtomicEditor/Release/D3DCompiler_47.dll",
        host.artifactsRoot + "AtomicEditor/D3DCompiler_47.dll");

      // We need some resources to run
      fs.copySync(atomicRoot + "Resources/CoreData",
        editorAppFolder + "Resources/CoreData");

      fs.copySync(atomicRoot + "Resources/EditorData",
        editorAppFolder + "Resources/EditorData");

      fs.copySync(atomicRoot + "Resources/PlayerData",
        editorAppFolder + "Resources/PlayerData");

      fs.copySync(atomicRoot + "Data/AtomicEditor",
        editorAppFolder + "Resources/ToolData");

      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
