var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var atomicRoot = bcommon.atomicRoot;
var buildDir = bcommon.artifactsRoot + "Build/Windows/";
var atomicToolBinary = buildDir + "Source/AtomicTool/Release/AtomicTool.exe";

namespace('build', function() {

  task('atomic', {
    async: true
  }, function() {

    bcommon.cleanCreateDir(buildDir);
    bcommon.cleanCreateDir(bcommon.getGenScriptRootDir("WINDOWS"));

    process.chdir(buildDir);

    var bindCmd = atomicToolBinary + " bind \"" + atomicRoot + "\" ";

    var cmds = [
      atomicRoot + "/Build/CIScripts/Windows/CompileAtomicTool.bat",
      bindCmd + "Script/Packages/Atomic/ WINDOWS",
      bindCmd + "Script/Packages/AtomicPlayer/ WINDOWS",
      bindCmd + "Script/Packages/ToolCore/ WINDOWS",
      bindCmd + "Script/Packages/Editor/ WINDOWS",
      bindCmd + "Script/Packages/WebView/ WINDOWS",
      atomicRoot + "/Build/CIScripts/Windows/CompileAtomic.bat",
    ]

    jake.exec(cmds, function() {

      var winEditorBinary = buildDir + "Source/AtomicEditor/Release/AtomicEditor.exe";
      var winEditorD3DBinary = buildDir + "Source/AtomicEditor/Release/D3DCompiler_47.dll";
      var winPlayerBinary = buildDir + "Source/AtomicPlayer/Application/Release/AtomicPlayer.exe";
      var winPlayerD3DBinary = buildDir + "Source/AtomicPlayer/Application/Release/D3DCompiler_47.dll";

      fs.copySync(winEditorBinary, buildDir + "Bin/AtomicEditor.exe");
      fs.copySync(winEditorD3DBinary, buildDir + "Bin/D3DCompiler_47.dll");
      fs.copySync(winPlayerBinary, buildDir + "Bin/AtomicPlayer.exe");

      console.log("Built Windows AtomicEditor & Player");

      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
