var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var buildDir = bcommon.artifactsRoot + "Build/IOS/";

namespace('build', function() {

  task('ios_player', {
    async: true
  }, function() {

    var atomicTool = host.getAtomicToolBinary();

    var playerBuildDir = buildDir + "AtomicPlayer/";

    bcommon.cleanCreateDir(playerBuildDir);
    bcommon.cleanCreateDir(bcommon.getGenScriptRootDir("IOS"));

    process.chdir(playerBuildDir);

    var cmds = [
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/Atomic/ IOS",
      atomicTool + " bind " + bcommon.atomicRoot + " Script/Packages/AtomicPlayer/ IOS",
      "cmake -DIOS=1 -G Xcode ../../../../"
    ];

    if (bcommon.jenkinsBuild) {

      cmds.push("security -v list-keychains -d system -s /Users/jenkins/Library/Keychains/codesign.keychain");
      cmds.push("security -v unlock-keychain /Users/jenkins/Library/Keychains/codesign.keychain");

    }

    cmds.push("xcodebuild -configuration Release -parallelizeTargets -jobs 4");

    jake.exec(cmds, function() {
      var iosPlayerBinary = playerBuildDir + "Source/AtomicPlayer/Application/Release-iphoneos/AtomicPlayer.app/AtomicPlayer";
      fs.copySync(iosPlayerBinary, buildDir + "Bin/AtomicPlayer");
      console.log("Built IOS Player");
      complete();

    }, {
      printStdout: true
    });

  });

}); // end of build namespace
