var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

var buildDir = host.artifactsRoot + "Build/Linux/";
var editorAppFolder = host.artifactsRoot + "AtomicEditor/";

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
    common.cleanCreateDir(host.getGenScriptRootDir("LINUX"));
  }

  // create the generated script files, so they will be picked up by cmake
  host.createGenScriptFiles("LINUX");

  process.chdir(buildDir);

  var cmds = [];

  cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -DCMAKE_BUILD_TYPE=Release");
  cmds.push("make -j4 GenerateScriptBindings")
  cmds.push("make -j4 AtomicEditor AtomicPlayer")

  jake.exec(cmds, function() {

      // Copy the Editor binaries
      fs.copySync(buildDir + "Source/AtomicEditor/AtomicEditor",
        host.artifactsRoot + "AtomicEditor/AtomicEditor");

      // We need some resources to run
      fs.copySync(atomicRoot + "Resources/CoreData",
        editorAppFolder + "Resources/CoreData");

      fs.copySync(atomicRoot + "Resources/PlayerData",
        editorAppFolder + "Resources/PlayerData");

      fs.copySync(atomicRoot + "Data/AtomicEditor",
        editorAppFolder + "Resources/ToolData");

      fs.copySync(atomicRoot + "Resources/EditorData",
        editorAppFolder + "Resources/EditorData");

      fs.copySync(atomicRoot + "Artifacts/Build/Resources/EditorData/AtomicEditor/EditorScripts",
        editorAppFolder + "Resources/EditorData/AtomicEditor/EditorScripts");

      fs.copySync(buildDir +  "Source/AtomicPlayer/Application/AtomicPlayer",
        editorAppFolder + "Resources/ToolData/Deployment/Linux/AtomicPlayer");

      // Copy CEF support files
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/icudtl.dat",
        editorAppFolder+"/icudtl.dat");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Release/natives_blob.bin",
        editorAppFolder+"/natives_blob.bin");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Release/snapshot_blob.bin",
        editorAppFolder+"/snapshot_blob.bin");

      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/locales", editorAppFolder+"/locales");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef.pak", editorAppFolder+"/cef.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef_100_percent.pak", editorAppFolder+"/cef_100_percent.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef_200_percent.pak", editorAppFolder+"/cef_200_percent.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef_extensions.pak", editorAppFolder+"/cef_extensions.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/devtools_resources.pak", editorAppFolder+"/devtools_resources.pak");

      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/icudtl.dat",  atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/icudtl.dat");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Release/natives_blob.bin", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/natives_blob.bin");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Release/snapshot_blob.bin", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/snapshot_blob.bin");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/locales", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/locales");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef.pak", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/cef.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef_100_percent.pak", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/cef_100_percent.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef_200_percent.pak", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/cef_200_percent.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/cef_extensions.pak", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/cef_extensions.pak");
      fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/devtools_resources.pak", atomicRoot +"/Artifacts/Build/Linux/Source/AtomicEditor/devtools_resources.pak");


    console.log("\n\nAtomic Editor build to " + editorAppFolder + "\n\n");

    complete();

  }, {
    printStdout: true
  });

});

});// end of build namespace
