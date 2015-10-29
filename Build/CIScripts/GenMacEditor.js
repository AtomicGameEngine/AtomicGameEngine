var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var atomicRoot = bcommon.atomicRoot;
var srcDir = bcommon.artifactsRoot + "Build/EditorData/";
var dstDir = bcommon.artifactsRoot + "Build/MacEditor/";

namespace('build', function() {

  task('genmaceditor', {
    async: true
  }, function() {

    bcommon.cleanCreateDir(dstDir);

    console.log("Generating Mac App Bundle");

    var editorAppFolder = dstDir + "AtomicEditor.app/Contents/";

    fs.copySync(srcDir + "MacApps/EditorApp",
      editorAppFolder);

    fs.copySync(srcDir + "EditorBinaries/Mac/AtomicEditor",
      editorAppFolder + "MacOS/AtomicEditor");

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
