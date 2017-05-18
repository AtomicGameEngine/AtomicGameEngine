var os = require('os');
var path = require('path');

// get the root folder
var atomicRoot = path.resolve(__dirname, "../..") + "/";

// patch in our local node_modules
process.env.NODE_PATH = atomicRoot + "Build/node_modules/";
require('module').Module._initPaths();

var fs = require('fs-extra');

// Load `jake` global
require('../node_modules/jake/lib/jake');

var config = require('./BuildConfig');
var host = require('./Host');
require('./BuildCommon');

var cmd = config._[0];

// Check that we're in a local repo and not a downloaded zip
if (!fs.existsSync(atomicRoot + ".git")) {
    console.log("\nBUILD ERROR:\n\nAtomic must be built from a local git clone for submodules and git build information.\n");
    console.log("Please see: https://github.com/AtomicGameEngine/AtomicGameEngine/wiki/Building-Atomic-from-Source");
    process.exit(1);
}

function printHelp() {

    console.log("\nAtomic Editor Build Script");
    console.log("--------------------------");
    console.log("--help          : This help text");
    console.log("--with-android  : Build with Android platform support");
    console.log("--with-ios      : Build with iOS platform support");
    console.log("--with-web      : Build with Web platform support");
    console.log("--debug         : Build debug version of the editor and associated platform runtimes");
    console.log("--noclean       : Do not clean before building, useful during development");
    console.log("--nonet         : Build without AtomicNET C# scripting support");
    console.log("--with-docs     : Build and install API documents into the editor (requires npm on path)");
    console.log("--noexamples    : Don't include examples with editor");
    console.log("--task=name     : Build the specified task (for development)");
    console.log("--package       : packages the editor to Artifacts/Dist");
    if (os.platform() == "win32") {
      console.log("--vs2015        : Build with VS2015");
      console.log("--vs2017        : Build with VS2017");
      console.log("--opengl        : Enable OpenGL renderer");
      console.log("--d3d11         : Enable DirectX 11 renderer");
    }

    console.log("--------------------------")

    process.exit(0);
}

if (config["help"]) {
    printHelp();
}

if (config["lint"]) {
    var lintTask = jake.Task['build:lint'];
    lintTask.invoke();
}

if (config["task"]) {

    var task = jake.Task[config["task"]];

    if (!task) {
        console.log("\nBUILD ERROR:\n\nUnknown task: " + config["task"] + "\n");
        process.exit(1);
    }

    cmd = "";
    task.invoke();
}

// Atomic Editor Build
if (cmd == "buildeditor") {

    console.log("\n\nBuilding Atomic Editor, this process will take a few minutes\n");

    var buildTask = jake.Task['build:atomiceditor'];

    if (config["with-android"]) {

        if (!process.env.ANDROID_NDK) {
            console.log("\nANDROID_NDK environment variable not set, exiting\n");
            process.exit(1);
        }
    }

    if (config["with-web"]) {

        if (!process.env.EMSCRIPTEN) {
            console.log("\nEMSCRIPTEN environment variable not set, consider running 'source /Path/To/emsdk_env.sh', exiting\n");
            process.exit(1);
        }
    }

    if (config["with-ios"]) {

        if (os.platform() != "darwin") {
            console.log("\niOS platform requires macOS, exiting\n");
            process.exit(1);
        }
    }

    if (config["d3d11"] && config["opengl"]) {

        if (os.platform() == "win32") {
            console.log("\nBoth DirectX 11 and OpenGL flags specified. Please choose only one at a time.\nExiting...\n");
            process.exit(1);
        }
    }

    if (config["d3d11"]) {

        if (os.platform() != "win32") {
            console.log("\nDirectX 11 build requires Windows, exiting\n");
            process.exit(1);
        }
        else {
            console.log("\nDirectX 11 build selected.\n");
        }
    }


    if (config["opengl"]) {

        if (os.platform() != "win32") {
            console.log("\nOpenGL flag ignored, OpenGL is default on non-Windows platforms anyway.\nContinuing...\n");
        }
        else {
            console.log("\nOpenGL build selected.\n");
        }
    }


    buildTask.invoke();

}
