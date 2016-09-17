var fs = require('fs-extra');
var path = require("path");
var spawnSync = require('child_process').spawnSync
var host = require("./Host");

var atomicRoot = host.atomicRoot;
var buildDir = host.artifactsRoot + "Build/Mac/";
var editorAppFolder = host.artifactsRoot + "/AtomicEditor/AtomicEditor.app/";
var resourceDest = editorAppFolder + "/Contents/Resources/"

var buildAtomicNET = false;
var debug = false;
var config = "Release";

function copyAtomicNET() {

    if (!buildAtomicNET)
        return;

    fs.copySync(atomicRoot + "Artifacts/AtomicNET/" + config,
    resourceDest + "ToolData/AtomicNET/" + config);

    fs.copySync(atomicRoot + "Script/AtomicNET/AtomicProject.json",
    resourceDest + "ToolData/AtomicNET/Build/Projects/AtomicProject.json");

}

function copyAtomicEditor() {

    fs.copySync(buildDir + "Source/AtomicEditor/" + config + "/AtomicEditor.app", editorAppFolder);

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
    var playerBinary =  buildDir +  "Source/AtomicPlayer/Application/" + config + "/AtomicPlayer.app/Contents/MacOS/AtomicPlayer";

    fs.copySync(playerBinary, resourceDest + "ToolData/Deployment/MacOS/AtomicPlayer.app/Contents/MacOS/AtomicPlayer");

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
        cmds.push("xcodebuild -target AtomicEditor -target AtomicPlayer -configuration " + config + " -parallelizeTargets -jobs 4")

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
        var ios = options["with-ios"] ? true : false;
        var cleanBuild = options["noclean"] ? false : true;
        var installDocs = options["with-docs"] ? true : false;
        var installExamples = options["with-examples"] ? true : false;
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
        removeDirs.push(host.artifactsRoot + "Build/IOS/");

        host.setupDirs(cleanBuild, createDirs, removeDirs);

        if (!options["nonet"]) {
            buildAtomicNET = spawnSync("which", ["xbuild"]).status == 1 ? false : true;
        }

        process.chdir(buildDir);

        var cmds = [];

        // Generate XCode project, AtomicTool binary, and script bindings
        cmds.push("cmake ../../../ -DATOMIC_DEV_BUILD=0 -G Xcode");
        cmds.push("xcodebuild -target GenerateScriptBindings -target AtomicNETNative -configuration " + config + " -parallelizeTargets -jobs 4")

        jake.exec(cmds, function() {

            var rootTask = jake.Task['build:atomiceditor_phase2'];
            var task = rootTask;

            // add optional build components in reverse order
            if (buildAtomicNET) {
                var netTask = jake.Task['build:atomicnet'];
                task.prereqs.push("build:atomicnet")
                task = netTask;
            }

            if (ios) {
                var iosTask = jake.Task['build:ios_native'];
                task.prereqs.push("build:ios_native")
                task = iosTask;
            }

            if (android) {
                var androidTask = jake.Task['build:android_native'];
                task.prereqs.push("build:android_native")
                task = androidTask;
            }

            rootTask.addListener('complete', function () {
                console.log("\n\nAtomic Editor built to " + editorAppFolder + "\n\n");

                if (installDocs) {
                    jake.Task['build:gendocs'].invoke();
                }

                if (installExamples) {
                    jake.Task['build:genexamples'].invoke();
                }

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
