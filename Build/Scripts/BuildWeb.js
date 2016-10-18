var os = require('os');
var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var config = require('./BuildConfig');

var atomicRoot = config.atomicRoot;
var buildDir = config.artifactsRoot + "Build/Web/";

namespace('build', function() {

    task('web_player', {
        async: true
    }, function() {

        host.setupDirs(!config["noclean"], [buildDir]);

        process.chdir(buildDir);

        var cmds = [];

        if (os.platform() == "win32") {

        }
        else {
            cmds.push("cmake -DEMSCRIPTEN=1 -DCMAKE_TOOLCHAIN_FILE=\"" + atomicRoot + "Build/CMake/Toolchains/emscripten.toolchain.cmake\" -DCMAKE_BUILD_TYPE=Release ../../../");
            cmds.push("make -j4");
        }

        jake.exec(cmds, function() {

            var editorResourceFolder = config.artifactsRoot + (os.platform() == "win32" ? "AtomicEditor/Resources/" : "AtomicEditor/AtomicEditor.app/Contents/Resources/");

            var webPlayerBinary = buildDir + "/Source/AtomicPlayer/Application/AtomicPlayer.js";
            var webPlayerMemFile = buildDir + "/Source/AtomicPlayer/Application/AtomicPlayer.html.mem";

            // Install Deployment
            fs.copySync(webPlayerBinary, editorResourceFolder + "ToolData/Deployment/Web/AtomicPlayer.js");
            fs.copySync(webPlayerMemFile, editorResourceFolder + "ToolData/Deployment/Web/AtomicPlayer.html.mem");

            // Install locally as well
            fs.copySync(webPlayerBinary, atomicRoot + "Data/AtomicEditor/Deployment/Web/AtomicPlayer.js");
            fs.copySync(webPlayerMemFile, atomicRoot + "Data/AtomicEditor/Deployment/Web/AtomicPlayer.html.mem");

            complete();

        }, {
            printStdout: true,
            printStderr: true
        });

    });

}); // end of build namespace
