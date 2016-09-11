
var os = require('os');

// Parse args
var options = require('./Minimist')(process.argv.slice(2));
var cmd = options._[0];

// Load `jake` global
require('../node_modules/jake/lib/jake');

// Load jake tasks, etc
var host = require('./Host');

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

// Atomic Editor Build
if (cmd == "buildeditor") {

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
