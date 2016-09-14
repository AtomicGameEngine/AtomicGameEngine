var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var spawnSync = require('child_process').spawnSync

var atomicRoot = host.atomicRoot;
var buildDir = host.artifactsRoot + "Build/Linux/";
var editorAppFolder = host.artifactsRoot + "AtomicEditor/";

var buildAtomicNET = false;
var debug = false;
var config = "Release";

function copyAtomicNET() {

    if (!buildAtomicNET)
        return;

    fs.copySync(atomicRoot + "Artifacts/AtomicNET/" + config,
    editorAppFolder + "Resources/ToolData/AtomicNET/" + config);

    fs.copySync(atomicRoot + "Script/AtomicNET/AtomicProject.json",
    editorAppFolder + "Resources/ToolData/AtomicNET/Build/Projects/AtomicProject.json");

}

function copyAtomicEditor() {

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


    if (buildAtomicNET) {
        copyAtomicNET();
    }

}

namespace('build', function() {

    task('atomiceditor_phase2', {
        async: true
    }, function() {

        process.chdir(buildDir);

        var cmds = [];
        cmds.push("make AtomicEditor AtomicPlayer -j2")

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

        var options = host.options;

        var android = options["with-android"] ? true : false;
        var cleanBuild = options["noclean"] ? false : true;
        debug = options["debug"] ? true : false;
        config = debug ? "Debug" : "Release";

        var createDirs = [];
        var removeDirs = [];

        // We clean atomicNET here as otherwise platform binaries would be deleted
        createDirs.push(host.artifactsRoot + "AtomicNET/");
        createDirs.push(buildDir);
        createDirs.push(editorAppFolder);
        createDirs.push(host.getGenScriptRootDir());

        removeDirs.push(host.artifactsRoot + "Build/Android/");

        host.setupDirs(cleanBuild, createDirs, removeDirs);

        // TODO: build box has old node
        if (spawnSync)
            buildAtomicNET = spawnSync("which", ["xbuild"]).status == 1 ? false : true;

        process.chdir(buildDir);

        var cmds = [];

        // Generate Atomic solution, AtomicTool binary, and script bindings
        cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -DCMAKE_BUILD_TYPE=" + config);
        cmds.push("make AtomicNETNative -j2")

        jake.exec(cmds, function() {

            var rootTask = jake.Task['build:atomiceditor_phase2'];
            var task = rootTask;

            // add optional build components in reverse order
            if (buildAtomicNET) {
                var netTask = jake.Task['build:atomicnet'];
                task.prereqs.push("build:atomicnet")
                task = netTask;
            }

            if (android) {
                var androidTask = jake.Task['build:android_native'];
                task.prereqs.push("build:android_native")
                task = androidTask;
            }

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
