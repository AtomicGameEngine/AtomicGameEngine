
var os = require('os');
var host = require("./Host");
var config = require("./BuildConfig");
require("./PackageEditor");

// return an object with package name keys and module name lists as values
function installBuildTasks(rootTask) {

    var task = rootTask;

    // add optional build components in reverse order

    if (config["with-docs"]) {
        var docTask = jake.Task['build:gendocs'];
        task.prereqs.push("build:gendocs")
        task = docTask;
    }

    if (config["with-examples"]) {
        var examplesTask = jake.Task['build:genexamples'];
        task.prereqs.push("build:genexamples")
        task = examplesTask;
    }

    if (config["with-atomicnet"]) {
        var netTask = jake.Task['build:atomicnet'];
        task.prereqs.push("build:atomicnet")
        task = netTask;
    }

    if (config["with-ios"]) {
        var iosTask = jake.Task['build:ios_native'];
        task.prereqs.push("build:ios_native")
        task = iosTask;
    }

    if (config["with-android"]) {
        var androidTask = jake.Task['build:android_native'];
        task.prereqs.push("build:android_native")
        task = androidTask;
    }

}

exports.installBuildTasks = installBuildTasks;
