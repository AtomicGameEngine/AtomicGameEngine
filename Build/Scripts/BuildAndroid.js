var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var os = require('os');

var atomicRoot = host.atomicRoot;
var buildDir = host.artifactsRoot + "Build/Android/";

namespace('build', function() {

    task('android_native', {
        async: true
    }, function() {

        var options = host.options;
        var cleanBuild = options["noclean"] ? false : true;
        var debug = options["debug"] ? true : false;
        var config = debug ? "Debug" : "Release";


        host.setupDirs(cleanBuild, [buildDir]);

        process.chdir(buildDir);

        var cmds = [];

        if (os.platform() == "win32") {
            cmds.push(atomicRoot + "Build/Scripts/Windows/CompileAndroid.bat " + config);
        }
        else {
            cmds.push("cmake -G \"Unix Makefiles\" -DCMAKE_TOOLCHAIN_FILE=../../../Build/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE="  + config + " ../../../");
            cmds.push("make -j4");
        }

        jake.exec(cmds, function() {

            var editorResourceFolder = host.artifactsRoot + (os.platform() == "win32" ? "AtomicEditor/Resources/" : "AtomicEditor/AtomicEditor.app/Contents/Resources/");

            // Install Deployment
            fs.copySync(buildDir + "Source/AtomicPlayer/Application/libAtomicPlayer.so",
            editorResourceFolder + "ToolData/Deployment/Android/libs/armeabi-v7a/libAtomicPlayer.so");

            complete();

        }, {
            printStdout: true,
            printStderr: true
        });

    });

}); // end of build namespace
