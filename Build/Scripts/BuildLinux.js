var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var config = require("./BuildConfig");
var buildTasks = require("./BuildTasks");

var atomicRoot = config.atomicRoot;
var buildDir = config.artifactsRoot + "Build/Linux/";
var editorAppFolder = config.artifactsRoot + "AtomicEditor/";

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
    fs.copySync(buildDir + "Source/AtomicEditor/AtomicEditor",
    config.artifactsRoot + "AtomicEditor/AtomicEditor");

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

    var binaryFiles = ["chrome-sandbox", "libcef.so", "natives_blob.bin", "snapshot_blob.bin"];

    var resourceFiles = ["cef.pak",
    "cef_100_percent.pak",
    "cef_200_percent.pak",
    "cef_extensions.pak",
    "devtools_resources.pak",
    "icudtl.dat",
    "locales"];

    for (var i = 0; i < binaryFiles.length; i++) {
        fs.copySync(atomicRoot + "Submodules/CEF/Linux/Release/" + binaryFiles[i], editorAppFolder+"/" + binaryFiles[i]);
    }

    for (var i = 0; i < resourceFiles.length; i++) {
        fs.copySync(atomicRoot + "Submodules/CEF/Linux/Resources/" + resourceFiles[i], editorAppFolder+"/" + resourceFiles[i]);
    }


    copyAtomicNET();

}

namespace('build', function() {

    task('atomiceditor_phase2', {
        async: true
    }, function() {

        process.chdir(buildDir);

        var cmds = ["make AtomicEditor AtomicPlayer -j2"];

        jake.exec(cmds, function() {

            copyAtomicEditor();

            complete();

        }, {
            printStdout: true
        });

    });
    // Builds a standalone Atomic Editor, which can be distributed out of build tree
    task('atomiceditor', {
        async: true
    }, function() {

        // We clean atomicNET here as otherwise platform binaries would be deleted
        var createDirs = [config.artifactsRoot + "AtomicNET/", buildDir, editorAppFolder, host.getGenScriptRootDir()];

        var removeDirs = [config.artifactsRoot + "Build/Android/"];

        host.setupDirs(!config.noclean, createDirs, removeDirs);

        process.chdir(buildDir);

        var cmds = [];

        // Generate Atomic solution, AtomicTool binary, and script bindings
        cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -DCMAKE_BUILD_TYPE=" + config["config"]);
        cmds.push("make AtomicNETNative -j2")

        jake.exec(cmds, function() {

            var rootTask = jake.Task['build:atomiceditor_phase2'];

            buildTasks.installBuildTasks(rootTask);

            rootTask.addListener('complete', function () {

               complete();

            });

            rootTask.invoke();

        }, {
            printStdout: true,
            printStderr: true
        });

    });


});// end of build namespace
