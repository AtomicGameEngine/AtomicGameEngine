var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var buildTasks = require("./BuildTasks");
var config = require('./BuildConfig');

var atomicRoot = config.atomicRoot;
var buildDir = config.artifactsRoot + "Build/Mac/";
var resourceDest = config.editorAppFolder + "/Contents/Resources/"

function copyAtomicNET() {

    if (!config["with-atomicnet"])
        return;

    fs.copySync(atomicRoot + "Artifacts/AtomicNET/" + config["config"],
    resourceDest + "ToolData/AtomicNET/" + config["config"]);

    fs.copySync(atomicRoot + "Script/AtomicNET/AtomicProject.json",
    resourceDest + "ToolData/AtomicNET/Build/Projects/AtomicProject.json");

}

function copyAtomicEditor() {

    fs.copySync(buildDir + "Source/AtomicEditor/" + config["config"] + "/AtomicEditor.app", config.editorAppFolder);

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
    var playerBinary =  buildDir +  "Source/AtomicPlayer/Application/" + config["config"] + "/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";

    fs.copySync(playerBinary, resourceDest + "ToolData/Deployment/MacOS/AtomicPlayer.app/Contents/MacOS/AtomicPlayer");

    copyAtomicNET();

}

namespace('build', function() {

    task('atomiceditor_phase2', {
        async: true
    }, function() {

        process.chdir(buildDir);

        var cmds = [];
        cmds.push("xcodebuild -target AtomicEditor -target AtomicPlayer -configuration " + config["config"] + " -parallelizeTargets -jobs 4")

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
        var createDirs = [config.artifactsRoot + "AtomicNET/", buildDir, config.editorAppFolder, host.getGenScriptRootDir()];
        var removeDirs = [config.artifactsRoot + "Build/Android/", config.artifactsRoot + "Build/IOS/"];

        host.setupDirs(!config.noclean, createDirs, removeDirs);

        process.chdir(buildDir);

        var cmds = [];

        // Generate XCode project, AtomicTool binary, and script bindings
        cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -G Xcode");
        cmds.push("xcodebuild -target GenerateScriptBindings -target AtomicNETNative -configuration " + config["config"] + " -parallelizeTargets -jobs 4")

        jake.exec(cmds, function() {

            var rootTask = jake.Task['build:atomiceditor_phase2'];

            buildTasks.installBuildTasks(rootTask);

            rootTask.addListener('complete', function () {

                console.log("\n\nAtomic Editor built to " + config.editorAppFolder + "\n\n");

                complete();
            });

            rootTask.invoke();

        }, {
            printStdout: true,
            printStderr: true
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
            printStdout: true,
            printStderr: true
        });

    });


});// end of build namespace
