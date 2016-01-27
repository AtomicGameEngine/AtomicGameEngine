var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var atomicRoot = bcommon.atomicRoot;
var srcDir = bcommon.artifactsRoot + "Build/EditorData/";
var dstDir = bcommon.artifactsRoot + "Build/WindowsEditor/";

namespace('build', function() {

  task('genwindowsinstaller', ["build:genwindowseditor"], {
    async: true
  }, function() {

    console.log("Creating Windows Installer");

    bcommon.cleanCreateDir(bcommon.artifactsRoot + "Build/WindowsInstaller");

    var installerName = "AtomicEditorSetup_" + bcommon.buildSHA + ".exe";
    var installerPath = bcommon.artifactsRoot + "Build/WindowsInstaller/" + installerName;

    var nsisDefines = "/DATOMIC_ROOT=" + atomicRoot;
    nsisDefines += " /DEDITOR_VERSION=1";
    nsisDefines += " /DINSTALLER_NAME=" + installerName;

    var makeNSISCmd = atomicRoot + "\\Build\\CIScripts\\Windows\\CreateInstaller.bat";

    makeNSISCmd += " " + nsisDefines + " " + atomicRoot + "/Build/CIScripts/Windows/Installer/AtomicEditor.nsi";

    var editorExe = dstDir + "AtomicEditor/AtomicEditor.exe";

    var pfxFile = process.env.ATOMIC_PFX_FILE;
    var pfxPW = process.env.ATOMIC_PFX_PW;

    var signBaseCmd = "signtool.exe sign /f " + pfxFile;
    signBaseCmd += " /p " + pfxPW;
    signBaseCmd += " /t http://timestamp.verisign.com/scripts/timestamp.dll";
    signBaseCmd += " /v ";

    var signEditorCmd = signBaseCmd + editorExe;
    var signInstallerCmd = signBaseCmd + installerPath;

    var cmds = [signEditorCmd, makeNSISCmd, signInstallerCmd];

    jake.exec(cmds, function() {
      complete();
    }, {
      printStdout: true
    });

  });

  task('genwindowseditor', {
    async: true
  }, function() {

    bcommon.cleanCreateDir(dstDir);

    process.chdir(srcDir);

    cmds = [ atomicRoot + "Build/Windows/7z/7z.exe x -y EditorData.zip"];

    jake.exec(cmds, function() {

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

      // CEF
      var cefRoot = atomicRoot + "Submodules/CEF/Windows/64bit/";

      fs.copySync(cefRoot + "Release", editorAppFolder);
      fs.copySync(cefRoot + "Resources", editorAppFolder);

      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
