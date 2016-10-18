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
    console.log("--with-examples : Install examples into the editor (require git on path)");
    console.log("--task=name     : Build the specified task (for development)");
    console.log("--package       : packages the editor to Artifacts/Dist");
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

    // simple build check for submodules not being initialized

    if (!fs.existsSync(config.atomicRoot + "Submodules/CEF/Windows")) {

        console.log("\nBUILD ERROR:\n\nSubmodules not initialized.  When cloning repository, please use:\ngit clone --recursive https://github.com/AtomicGameEngine/AtomicGameEngine\n")
        process.exit(1);
    }

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

    buildTask.invoke();

}
