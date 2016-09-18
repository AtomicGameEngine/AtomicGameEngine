var os = require('os');
var fs = require('fs-extra');
var path = require("path");
var host = require("./Host");
var config = require("./BuildConfig");

var jenkinsBuild = process.env.ATOMIC_JENKINS_BUILD == 1;

var atomicRoot = config.atomicRoot;
var buildDir = config.artifactsRoot + "Build/IOS/";

namespace('build', function() {

    task('ios_native', {
        async: true
    }, function() {

        var NETNativeSrcDir = buildDir + "Source/AtomicNET/NETNative/" + config["config"] + "-iphoneos/";
        var NETNativeDestDir = config.artifactsRoot + "AtomicNET/" + config["config"] + "/Native/iOS/";

        host.setupDirs(!config["noclean"], [buildDir, NETNativeDestDir]);

        process.chdir(buildDir);

        var cmds = [];

        cmds.push("cmake -DIOS=1 -DATOMIC_DEV_BUILD=0 -G Xcode ../../../");

        if (jenkinsBuild) {
            cmds.push("security -v list-keychains -d system -s /Users/jenkins/Library/Keychains/codesign.keychain");
            cmds.push("security -v unlock-keychain /Users/jenkins/Library/Keychains/codesign.keychain");
        }

        cmds.push("xcodebuild -configuration " + config["config"] + " -parallelizeTargets -jobs 4");
        // Note that this install_name_tool invocation invalidates the code signing, Xamarin/Visual Studio should resign the binary on deploy to device
        cmds.push("cd \"" + NETNativeSrcDir + "\" && install_name_tool -id @rpath/AtomicNETNative.framework/AtomicNETNative AtomicNETNative.framework/AtomicNETNative");
        //cmds.push("cd \"" + NETNativeSrcDir + "\" && codesign --deep --force --verify --sign \"iPhone Developer\" ./AtomicNETNative.framework/");
        cmds.push("cd \"" + NETNativeSrcDir + "\" && zip -r AtomicNETNative.framework.zip AtomicNETNative.framework");

        jake.exec(cmds, function() {

            fs.copySync(NETNativeSrcDir + "AtomicNETNative.framework", NETNativeDestDir + "AtomicNETNative.framework");
            fs.copySync(NETNativeSrcDir + "AtomicNETNative.framework.zip", NETNativeDestDir + "AtomicNETNative.framework.zip");

            complete();

        }, {
            printStdout: true,
            printStderr: true
        });

    });

}); // end of build namespace
