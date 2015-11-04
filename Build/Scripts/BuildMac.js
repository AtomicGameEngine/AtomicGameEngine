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

  // create the generated script files, so they will be picked up by cmake
  host.createGenScriptFiles("MACOSX");

  process.chdir(buildDir);

  var cmds = [];

  cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -G Xcode");
  cmds.push("xcodebuild -target GenerateScriptBindings -configuration Release")
  cmds.push("xcodebuild -target AtomicEditor -target AtomicPlayer -configuration Release")

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

  // create the generated script files, so they will be picked up by cmake
  host.createGenScriptFiles("MACOSX");

  process.chdir(xcodeRoot);

  var cmds = [];

  cmds.push("cmake ../AtomicGameEngine -DATOMIC_DEV_BUILD=1 -G Xcode");
  cmds.push("xcodebuild -target GenerateScriptBindings -configuration Debug")

  jake.exec(cmds, function() {

    var task = jake.Task['build:genscripts']

    task.addListener('complete', function () {
        console.log("\n\nXCode workspace generated in " +  xcodeRoot + "\n\n");
        complete();
      });

    task.invoke("MACOSX");

  }, {
    printStdout: true
  });

});


});// end of build namespace
