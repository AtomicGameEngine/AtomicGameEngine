var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var atomicRoot = bcommon.atomicRoot;
var srcDir = bcommon.artifactsRoot + "Build/EditorData/";
var dstDir = bcommon.artifactsRoot + "Build/WindowsEditor/";

namespace('build', function() {

  task('genwindowseditor', {
    async: true
  }, function() {

    bcommon.cleanCreateDir(dstDir);

    console.log("Generating Windows Editor");

    var editorAppFolder = dstDir + "AtomicEditor/";

    fs.copySync(srcDir + "EditorBinaries/Windows/AtomicEditor.exe",
      editorAppFolder + "AtomicEditor.exe");

    fs.copySync(srcDir + "EditorBinaries/Windows/D3DCompiler_47.dll",
      editorAppFolder + "D3DCompiler_47.dll");

    fs.copySync(srcDir + "Resources/CoreData",
      editorAppFolder + "Resources/CoreData");

    fs.copySync(srcDir + "Resources/EditorData",
      editorAppFolder + "Resources/EditorData");

    fs.copySync(srcDir + "Resources/PlayerData",
      editorAppFolder + "Resources/PlayerData");

    fs.copySync(srcDir + "Resources/ToolData",
      editorAppFolder + "Resources/ToolData");

    fs.copySync(srcDir + "AtomicExamples",
      editorAppFolder + "Resources/ToolData/AtomicExamples");

    fs.copySync(srcDir + "Docs",
      editorAppFolder + "Resources/ToolData/Docs/JSDocs");

    complete();


  });

}); // end of build namespace
