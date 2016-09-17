var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");

var atomicRoot = host.atomicRoot;
var buildDir = host.artifactsRoot + "Build/Windows/";
var editorAppFolder = host.artifactsRoot + "AtomicEditor/";

var buildAtomicNET = true;
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
    fs.copySync(buildDir + "Source/AtomicEditor/" + config,
    host.artifactsRoot + "AtomicEditor");

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

    fs.copySync(buildDir +  "Source/AtomicPlayer/Application/" + config +"/AtomicPlayer.exe",
    editorAppFolder + "Resources/ToolData/Deployment/Windows/x64/AtomicPlayer.exe");

    fs.copySync(buildDir +  "Source/AtomicPlayer/Application/" + config + "/D3DCompiler_47.dll",
    editorAppFolder + "Resources/ToolData/Deployment/Windows/x64/D3DCompiler_47.dll");

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
        cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAtomicEditorPhase2.bat " + config);

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

        host.setupDirs(cleanBuild, createDirs, removeDirs);

        process.chdir(buildDir);

        var cmds = [];

        // Generate Atomic solution, AtomicTool binary, and script bindings
        cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAtomicEditorPhase1.bat " + config);

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

    // Generate a Visual Studio 2015 solution
    task('genvs2015', {
        async: true
    }, function(devBuild) {
        if (devBuild === undefined)
        devBuild = 1;

        var slnRoot = path.resolve(atomicRoot, "") + "-VS2015\\";

        if (!fs.existsSync(slnRoot)) {
            jake.mkdirP(slnRoot);
        }

        process.chdir(slnRoot);

        var cmds = [];

        cmds.push(atomicRoot + "Build/Scripts/Windows/GenerateVS2015.bat " + atomicRoot + " " + devBuild);

        jake.exec(cmds, function() {

            complete();

        }, {
            printStdout: true
        });

    });


});// end of build namespace
