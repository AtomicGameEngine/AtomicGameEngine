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

      bcommon.cleanCreateDir(dstDir + "MacEditorInstaller");

      var editorAppFolder = dstDir + "AtomicEditor.app";

      cmds = [
      "chmod +x " + editorAppFolder + "/Contents/MacOS/AtomicEditor",
      "chmod +x " + editorAppFolder + "/Contents/Resources/ToolData/Deployment/MacOS/AtomicPlayer.app/Contents/MacOS/AtomicPlayer",
      "security unlock-keychain -p \"jenkins\" /Users/jenkins/Library/Keychains/login.keychain  ",
      "codesign --force --verify --verbose --sign \"Developer ID Application: THUNDERBEAST GAMES LLC (C7M5MAQQWR)\" " + editorAppFolder + "/Contents/Frameworks/AtomicEditor\\ Helper.app",
      "codesign --force --verify --verbose --sign \"Developer ID Application: THUNDERBEAST GAMES LLC (C7M5MAQQWR)\" " + editorAppFolder + "/Contents/Frameworks/Chromium\\ Embedded\\ Framework.framework",
      "codesign --force --verify --verbose --sign \"Developer ID Application: THUNDERBEAST GAMES LLC (C7M5MAQQWR)\" " + editorAppFolder,
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

    cmds = ["unzip " + srcDir + "EditorData.zip -d " + srcDir,
            "unzip " + srcDir + "EditorBinaries/Mac/AtomicEditor.zip -d " + srcDir + "EditorBinaries/Mac"];

    jake.exec(cmds, function() {

      console.log("Generating Mac App Bundle");

      var editorAppFolder = dstDir + "AtomicEditor.app/Contents/";

      fs.copySync(srcDir + "EditorBinaries/Mac/AtomicEditor.app",
        dstDir + "AtomicEditor.app");

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

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
