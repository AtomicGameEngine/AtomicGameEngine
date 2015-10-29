var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var atomicRoot = bcommon.atomicRoot;
var srcDir = bcommon.artifactsRoot + "Build/EditorData/";
var dstDir = bcommon.artifactsRoot + "Build/MacEditor/";

namespace('build', function() {

  task('genmaceditorinstaller', ["genmaceditor"], {
    async: true
  }, function() {

      var editorAppFolder = dstDir + "AtomicEditor.app";

      cmds = [
      "security -v list-keychains -d system -s /Users/jenkins/Library/Keychains/codesign.keychain",
      "security -v unlock-keychain -p \"\" /Users/jenkins/Library/Keychains/codesign.keychain",
      "codesign -s \"THUNDERBEAST GAMES LLC\" \"" + editorAppFolder +  "\"",
      "cd " + dstDir + " && zip -r -X " + "./MacEditorInstaller/AtomicEditor_MacOSX_" + bcommon.buildSHA + ".zip ./AtomicEditor.app"

    ];

    jake.exec(cmds, function() {

      console.log("Built Mac Editor");

      complete();

    }, {
      printStdout: true
    });


  });

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
