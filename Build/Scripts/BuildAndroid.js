var os = require('os');
var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var config = require('./BuildConfig');

var atomicRoot = config.atomicRoot;
var buildDir = config.artifactsRoot + "Build/Android/";

namespace('build', function() {

    task('android_native', {
        async: true
    }, function() {

        host.setupDirs(!config["noclean"], [buildDir]);

        process.chdir(buildDir);

        var cmds = [];

        if (os.platform() == "win32") {
            cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAndroid.bat " + config["config"]);
        }
        else {
            cmds.push("cmake -G \"Unix Makefiles\" -DCMAKE_TOOLCHAIN_FILE=../../../Build/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE="  + config["config"] + " ../../../");
            cmds.push("make -j4");
        }

        jake.exec(cmds, function() {

            var editorResourceFolder = config.artifactsRoot + (os.platform() == "win32" ? "AtomicEditor/Resources/" : "AtomicEditor/AtomicEditor.app/Contents/Resources/");

            // Install Deployment
            fs.copySync(buildDir + "Source/AtomicPlayer/Application/libAtomicPlayer.so",
            editorResourceFolder + "ToolData/Deployment/Android/libs/armeabi-v7a/libAtomicPlayer.so");

            // Install local deployment
            fs.copySync(buildDir + "Source/AtomicPlayer/Application/libAtomicPlayer.so",
            atomicRoot + "Artifacts/AtomicEditor/Resources/ToolData/Deployment/Android/libs/armeabi-v7a/libAtomicPlayer.so");

            complete();

        }, {
            printStdout: true,
            printStderr: true
        });

    });

}); // end of build namespace
