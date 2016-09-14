var os = require('os');
var fs = require('fs-extra');

// Load `jake` global
require('../node_modules/jake/lib/jake');

// Load jake tasks, patch in our node modules, etc
var host = require('./Host');

// Parse args
var options = require('minimist')(process.argv.slice(2));
var cmd = options._[0];



// Make options availabe to host
host.options = options;

function printHelp() {

    console.log("\nAtomic Editor Build Script")
    console.log("--------------------------")
    console.log("--help         : This help text")
    console.log("--with-android : Build with Android platform support");
    console.log("--with-ios     : Build with iOS platform support");
    console.log("--debug        : Build debug version of the editor and associated platform runtimes")
    console.log("--noclean      : Do not clean before building, useful during development")
    console.log("--nonet        : Build without AtomicNET C# scripting support")
    console.log("--------------------------")

    process.exit(0);
}

if (options["help"]) {
    printHelp();
}

if (options["lint"]) {
    var lintTask = jake.Task['build:lint'];
    lintTask.invoke();
}

// Atomic Editor Build
if (cmd == "buildeditor") {

    // simple build check for submodules not being initialized

    if (!fs.existsSync(host.atomicRoot + "Submodules/CEF/Windows")) {

        console.log("\nBUILD ERROR:\n\nSubmodules not initialized.  When cloning repository, please use:\ngit clone --recursive https://github.com/AtomicGameEngine/AtomicGameEngine\n")
        process.exit(1);
    }

    console.log("\n\nBuilding Atomic Editor, this process will take a few minutes\n");

    var buildTask = jake.Task['build:atomiceditor'];

    if (options["with-android"]) {

        if (!process.env.ANDROID_NDK) {
            console.log("\nANDROID_NDK environment variable not set, exiting\n");
            process.exit(1);
        }
    }

    if (options["with-ios"]) {

        if (os.platform() != "darwin") {
            console.log("\niOS platform requires macOS, exiting\n");
            process.exit(1);
        }
    }

    buildTask.invoke();

}
