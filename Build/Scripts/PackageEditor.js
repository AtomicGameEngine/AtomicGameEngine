var fs = require('fs-extra');
var host = require("./Host");
var config = require("./BuildConfig");

namespace('package', function() {

    // TODO: Look into if possible to archive on jenkins with different base dir than workspace
    var webplayerSrcDir = config.artifactsRoot + "DistGen/Artifacts/AtomicEditor/AtomicEditor.app/Contents/Resources/ToolData/Deployment/Web";

    task('windows_editor', {
        async: true
    }, function() {

        var srcDir = config.artifactsRoot + "AtomicEditor/";
        var dstDir = config.artifactsRoot + "Dist/";

        host.cleanCreateDir(dstDir);

        // Copy WebGL CI artifacts into editor package
        if (fs.existsSync(webplayerSrcDir)) {

            fs.copySync(webplayerSrcDir,
            config.artifactsRoot + "/AtomicEditor/Resources/ToolData/Deployment/Web", {clobber : true });

        }

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
        var certSubjectName = process.env.ATOMIC_CERT_SUBJECTNAME;
        var codesign = false;

        if (certSubjectName || (pfxFile && pfxPW)) {
          codesign = true;
        }

        var signBaseCmd = "signtool.exe sign";

        if (certSubjectName.indexOf(" ") != -1) {
          var original = certSubjectName;
          certSubjectName = certSubjectName.split(" ")[0];
          console.log("CodeSign: ATOMIC_CERT_SUBJECTNAME: \"" + original + "\" contains a space, changed to " +
                       certSubjectName + " to avoid jake.exec quote issue on Windows");
        }

        if (certSubjectName) {
          signBaseCmd += " /tr http://timestamp.digicert.com /td sha256 /fd sha256";
          signBaseCmd += " /n " + certSubjectName + " ";
        } else {

          signBaseCmd += " /f " + pfxFile;
          signBaseCmd += " /p " + pfxPW;
          signBaseCmd += " /t http://timestamp.verisign.com/scripts/timestamp.dll";
          signBaseCmd += " /v ";

        }

        var signEditorCmd = signBaseCmd + config.artifactsRoot + "AtomicEditor/AtomicEditor.exe";
        var signInstallerCmd = signBaseCmd + installerPath;

        var cmds = [];

        if (codesign) {
            cmds.push(signEditorCmd);
        }

        cmds.push(makeNSISCmd);

        if (codesign) {
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

        // Copy WebGL CI artifacts into editor package
        if (fs.existsSync(webplayerSrcDir)) {
            fs.copySync(webplayerSrcDir,
            config.artifactsRoot + "/AtomicEditor/AtomicEditor.app/Contents/Resources/ToolData/Deployment/Web", {clobber : true });
        }

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

    task('linux_editor', {
        async: true
    }, function() {

        var editorAppFolder = config.editorAppFolder;
        var srcDir = config.artifactsRoot + "AtomicEditor/";
        var dstDir = config.artifactsRoot + "Dist/";
        var dstDeb = config.artifactsRoot + "AtomicEditor_LINUX_" + config.buildSHA + ".deb";

        host.cleanCreateDir(dstDir);  // create new staging directory
        fs.removeSync(dstDeb);  // remove old one, if there

        // copy in the two magic dirs
        fs.copySync(config.atomicRoot + "Build/Linux/DEBIAN", dstDir + "DEBIAN");
        fs.copySync(config.atomicRoot + "Build/Linux/usr", dstDir + "usr" );

        // copy in the atomic dir
        fs.copySync(editorAppFolder, dstDir + "usr/share/atomicgameengine" );

        //copy in menu pixmap
        fs.copySync(config.atomicRoot + "Build/Linux/atomic_menu.xpm", dstDir + "usr/share/atomicgameengine/atomic_menu.xpm" );

        //copy in copyright
        fs.copySync(config.atomicRoot + "LICENSE.md", dstDir + "usr/share/doc/atomicgameengine/copyright" );

        cmds = [];

        // go to staging root directory
        cmds.push("cd " + config.artifactsRoot + " ;" );

        // get rid of some lintian errors
        cmds.push("find " + dstDir + "usr/share/atomicgameengine/ -name .gitignore -type f -delete ;");
        cmds.push("find " + dstDir + "usr/share/atomicgameengine/ -maxdepth 9 -type f -print0 | xargs -0 chmod oug-x ;");
        cmds.push("/bin/chmod oug+x " + dstDir + "usr/share/atomicgameengine/AtomicEditor ;");
        cmds.push("/bin/chmod oug+x " + dstDir + "usr/share/atomicgameengine/Resources/ToolData/Deployment/Linux/AtomicPlayer ;" );
        cmds.push("/usr/bin/strip --strip-unneeded " + dstDir + "usr/share/atomicgameengine/AtomicEditor ;" );
        cmds.push("/usr/bin/strip --strip-unneeded " + dstDir + "usr/share/atomicgameengine/Resources/ToolData/Deployment/Linux/AtomicPlayer ;" );

        // create the package
        // needs fakeroot for package file ownership issues
        cmds.push("/usr/bin/fakeroot /usr/bin/dpkg-deb --build " + dstDir + ";");

        // change the deb name, following Atomic standards
        cmds.push("/bin/mv " + config.artifactsRoot + "Dist.deb " + dstDeb + " ;");

        // clean up the staging area
        cmds.push("/bin/rm -rf "+  dstDir + " ;");

        jake.exec(cmds, function() {

          console.log("Packaged Linux Editor to " + dstDeb );

          complete();

        }, {
          printStdout: true,
          printStderr: true
        });

    });

});
