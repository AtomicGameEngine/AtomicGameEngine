var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

var buildDir = host.artifactsRoot + "Build/Mac/";
var editorAppFolder = host.artifactsRoot + "/AtomicEditor/AtomicEditor.app/";

namespace('build', function() {

// Builds a standalone Atomic Editor, which can be distributed out of build tree
task('atomiceditor', {
  async: true
}, function() {

  // Clean build
  var cleanBuild = true;
  if (cleanBuild) {
    common.cleanCreateDir(buildDir);
    common.cleanCreateDir(editorAppFolder);
    common.cleanCreateDir(host.getGenScriptRootDir("MACOSX"));
  }

  process.chdir(buildDir);

  var cmds = [];

  cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -G Xcode");
  cmds.push("xcodebuild -target AtomicEditor -target AtomicPlayer -configuration Release -parallelizeTargets -jobs 4")

  jake.exec(cmds, function() {

    fs.copySync(buildDir + "Source/AtomicEditor/Release/AtomicEditor.app", editorAppFolder);

    var resourceDest = editorAppFolder + "/Contents/Resources/"

    // We need some resources to run
    fs.copySync(atomicRoot + "Resources/CoreData",
      resourceDest + "CoreData");

    fs.copySync(atomicRoot + "Resources/PlayerData",
      resourceDest + "PlayerData");

    fs.copySync(atomicRoot + "Data/AtomicEditor",
      resourceDest + "ToolData");

    fs.copySync(atomicRoot + "Resources/EditorData",
      resourceDest + "EditorData");

    fs.copySync(atomicRoot + "Artifacts/Build/Resources/EditorData/AtomicEditor/EditorScripts",
      resourceDest + "EditorData/AtomicEditor/EditorScripts");

    // copy the mac player binary to deployment
    var playerBinary =  buildDir +  "Source/AtomicPlayer/Application/Release/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";

    fs.copySync(playerBinary,
      resourceDest + "ToolData/Deployment/MacOS/AtomicPlayer.app/Contents/MacOS/AtomicPlayer");

    console.log("\n\nAtomic Editor build to " + editorAppFolder + "\n\n");

    complete();

  }, {
    printStdout: true
  });

});

// Generate a XCode Workspace
task('genxcode', {
  async: true
}, function() {

  var xcodeRoot = path.resolve(atomicRoot, "") + "-XCode";

  if (!fs.existsSync(xcodeRoot)) {
      jake.mkdirP(xcodeRoot);
  }

  process.chdir(xcodeRoot);

  var cmds = [];

  cmds.push("cmake ../AtomicGameEngine -DATOMIC_DEV_BUILD=1 -G Xcode");

  jake.exec(cmds, function() {

    complete();

  }, {
    printStdout: true
  });

});


});// end of build namespace
