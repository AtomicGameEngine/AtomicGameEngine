var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var atomicRoot = host.atomicRoot;

var buildDir = host.artifactsRoot + "Build/Windows/";
var editorAppFolder = host.artifactsRoot + "AtomicEditor/";

namespace('build', function() {

    // Builds a standalone Atomic Editor, which can be distributed out of build tree
    task('atomiceditor', {
        async: true
    }, function(android) {

        android = android == "android" ? true : false;

        // Clean build
        var cleanBuild = true;
        if (cleanBuild) {
            common.cleanCreateDir(host.artifactsRoot + "AtomicNET/");
            common.cleanCreateDir(buildDir);
            common.cleanCreateDir(editorAppFolder);
            common.cleanCreateDir(host.getGenScriptRootDir());
        }

        process.chdir(buildDir);

        var cmds = [];

        // Build the AtomicEditor
        cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAtomicEditor.bat");
        cmds.push(host.atomicTool + " net compile " + atomicRoot + "Script/AtomicNET/AtomicNETProject.json " + (android ? "ANDROID" : "WINDOWS") + " Release");

        function copyAtomicNET() {

            fs.copySync(atomicRoot + "Artifacts/AtomicNET/Release",
            editorAppFolder + "Resources/ToolData/AtomicNET/Release");

            fs.copySync(atomicRoot + "Script/AtomicNET/AtomicProject.json",
            editorAppFolder + "Resources/ToolData/AtomicNET/Build/Projects/AtomicProject.json");

        }

        jake.exec(cmds, function() {

            // Copy the Editor binaries
            fs.copySync(buildDir + "Source/AtomicEditor/Release",
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

            fs.copySync(buildDir +  "Source/AtomicPlayer/Application/Release/AtomicPlayer.exe",
            editorAppFolder + "Resources/ToolData/Deployment/Windows/x64/AtomicPlayer.exe");

            fs.copySync(buildDir +  "Source/AtomicPlayer/Application/Release/D3DCompiler_47.dll",
            editorAppFolder + "Resources/ToolData/Deployment/Windows/x64/D3DCompiler_47.dll");

            if (android) {

                var androidNativeTask = jake.Task['build:android_native'];

                androidNativeTask.addListener('complete', function () {
                    copyAtomicNET();
                    console.log("\nAtomic Editor build to ", editorAppFolder);
                    complete();
                });

                androidNativeTask.invoke();

            }
            else {
                copyAtomicNET();
                console.log("\nAtomic Editor build to ", editorAppFolder);
                complete();
            }

        }, {
            printStdout: true
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

}); // end of build namespace
