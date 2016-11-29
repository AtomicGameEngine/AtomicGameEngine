var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var buildTasks = require("./BuildTasks");
var config = require("./BuildConfig");

var atomicRoot = config.atomicRoot;
var buildDir = config.artifactsRoot + "Build/Windows/";
var editorAppFolder = config.editorAppFolder

function copyAtomicNET() {

    if (!config["with-atomicnet"])
        return;

    fs.copySync(atomicRoot + "Artifacts/AtomicNET/" + config["config"],
    editorAppFolder + "Resources/ToolData/AtomicNET/" + config["config"]);

    fs.copySync(atomicRoot + "Script/AtomicNET/AtomicProject.json",
    editorAppFolder + "Resources/ToolData/AtomicNET/Build/Projects/AtomicProject.json");

}

function copyAtomicEditor() {

    // Copy the Editor binaries
    fs.copySync(buildDir + "Source/AtomicEditor/" + config["config"],
    config.artifactsRoot + "AtomicEditor");

    // copy AtomicTool
    fs.copySync(buildDir +  "Source/AtomicTool/" + config["config"] +"/AtomicTool.exe",
    editorAppFolder + "AtomicTool.exe");

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

    fs.copySync(buildDir +  "Source/AtomicPlayer/Application/" + config["config"] +"/AtomicPlayer.exe",
    editorAppFolder + "Resources/ToolData/Deployment/Windows/x64/AtomicPlayer.exe");

    fs.copySync(buildDir +  "Source/AtomicPlayer/Application/" + config["config"] + "/D3DCompiler_47.dll",
    editorAppFolder + "Resources/ToolData/Deployment/Windows/x64/D3DCompiler_47.dll");

    copyAtomicNET();

}

namespace('build', function() {

    // get CMake flags for generator, vsver parameter can be VS2017/VS2015, etc
    function getCMakeFlags(vsver) {

      var flags = "\"";

      // Redistributable editor build
      flags += "-DATOMIC_DEV_BUILD=0";

      // graphics backend (overrides default DX9)
      flags += " -DATOMIC_OPENGL=" + (config["opengl"] ? "ON" : "OFF");
      flags += " -DATOMIC_D3D11=" + (config["d3d11"] ? "ON" : "OFF");

      flags += "\"";

      return flags;

    }

    task('atomiceditor_phase2', {
        async: true
    }, function() {

        process.chdir(buildDir);

        var vsver = (config["vs2017"] ? "VS2017" : "VS2015");

        var cmds = [];
        cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAtomicEditorPhase2.bat " + config["config"] + " " + vsver);

        jake.exec(cmds, function() {

            copyAtomicEditor();

            if (config.package) {

                jake.Task['package:windows_editor'].invoke();

            }

            complete();

        }, {
            printStdout: true
        });

    });
    // Builds a standalone Atomic Editor, which can be distributed out of build tree
    task('atomiceditor', {
        async: true
    }, function() {

        // Always cleanly create the editor target folder
        host.cleanCreateDir(editorAppFolder);

        // We clean atomicNET here as otherwise platform binaries would be deleted
        var createDirs = [config.artifactsRoot + "AtomicNET/", buildDir, host.getGenScriptRootDir()];

        var removeDirs = [config.artifactsRoot + "Build/Android/"];

        host.setupDirs(!config.noclean, createDirs, removeDirs);

        process.chdir(buildDir);

        var vsver = (config["vs2017"] ? "VS2017" : "VS2015");

        var cmds = [];

        // Generate Atomic solution, AtomicTool binary, and script bindings
        cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAtomicEditorPhase1.bat " + config["config"] + " " +
                  vsver + " " + getCMakeFlags(vsver));

        jake.exec(cmds, function() {

            var rootTask = jake.Task['build:atomiceditor_phase2'];

            buildTasks.installBuildTasks(rootTask);

            rootTask.addListener('complete', function () {

                console.log("\n\nAtomic Editor built to " + editorAppFolder + "\n\n");

                complete();
            });

            rootTask.invoke();

        }, {
            printStdout: true,
            printStderr: true
        });

    });

});// end of build namespace
