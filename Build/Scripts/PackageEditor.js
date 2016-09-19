var host = require("./Host");
var config = require("./BuildConfig");

namespace('package', function() {

    task('windows_editor', {
        async: true
    }, function() {

        var srcDir = config.artifactsRoot + "AtomicEditor/";
        var dstDir = config.artifactsRoot + "Dist/";

        host.cleanCreateDir(dstDir);

        var installerName = "AtomicEditor_Windows64_Setup_" + config.buildSHA + ".exe";
        var installerPath = config.artifactsRoot + "Dist/" + installerName;

        var nsisDefines = "/DATOMIC_ROOT=" + config.atomicRoot;
        nsisDefines += " /DEDITOR_VERSION=1";
        nsisDefines += " /DINSTALLER_NAME=" + installerName;

        var makeNSISCmd = config.atomicRoot + "\\Build\\Scripts\\Windows\\CreateInstaller.bat";
        makeNSISCmd += " " + nsisDefines + " " + config.atomicRoot + "/Build/Scripts/Windows/Installer/AtomicEditor.nsi";
        var editorExe = dstDir + "/AtomicEditor.exe";

        var pfxFile = process.env.ATOMIC_PFX_FILE;
        var pfxPW = process.env.ATOMIC_PFX_PW;

        var signBaseCmd = "signtool.exe sign /f " + pfxFile;
        signBaseCmd += " /p " + pfxPW;
        signBaseCmd += " /t http://timestamp.verisign.com/scripts/timestamp.dll";
        signBaseCmd += " /v ";

        var signEditorCmd = signBaseCmd + config.artifactsRoot + "AtomicEditor/AtomicEditor.exe";
        var signInstallerCmd = signBaseCmd + installerPath;

        var cmds = [];

        if (pfxFile && pfxPW) {
            cmds.push(signEditorCmd);
        }

        cmds.push(makeNSISCmd);

        if (pfxFile && pfxPW) {
            cmds.push(signInstallerCmd);
        }

        jake.exec(cmds, function() {
          complete();
        }, {
          printStdout: true
        });


    });

    task('mac_editor', {
        async: true
    }, function() {


        var editorAppFolder = config.editorAppFolder;
        var srcDir = config.artifactsRoot + "AtomicEditor/";
        var dstDir = config.artifactsRoot + "Dist/";
        var editorZip = dstDir + "AtomicEditor_MacOSX_" +  config.buildSHA + ".zip";
        var devIDApp = config.devIDApp;

        host.cleanCreateDir(dstDir);

        cmds = [];

        if (config.jenkins) {
            cmds.push("security unlock-keychain -p \"jenkins\" /Users/jenkins/Library/Keychains/login.keychain  ");
        }

        if (devIDApp) {
            cmds.push("codesign --deep --force --verify --verbose --sign \"Developer ID Application: " + devIDApp + "\" " + editorAppFolder);
        }

        cmds.push("cd " + srcDir + " && zip -r -X " + editorZip +" ./AtomicEditor.app");

        jake.exec(cmds, function() {

          console.log("Packaged Mac Editor to ", editorZip);

          if (!devIDApp) {
              console.log("\nDeveloper ID Application not specified, code is not be signed!\n\n");
          }

          complete();

        }, {
          printStdout: true,
          printStderr: true
        });

    });

});
