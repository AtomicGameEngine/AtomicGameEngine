var fs = require('fs');

var jakeRoot = __dirname;

var artifactsFolder = jakeRoot + "/Artifacts";

// cmake
var cmakeDevBuild = true;

// build folder
var windowsBuildFolder = artifactsFolder + "/Windows_Build";
var macOSXBuildFolder = artifactsFolder + "/MacOSX_Build";
var androidBuildFolder = artifactsFolder + "/Android_Build";
var iosBuildFolder = artifactsFolder + "/IOS_Build";
var webBuildFolder = artifactsFolder + "/Web_Build";
var linuxBuildFolder = artifactsFolder + "/Linux_Build";

var allBuildFolders = [
  windowsBuildFolder,
  macOSXBuildFolder,
  androidBuildFolder,
  iosBuildFolder,
  webBuildFolder,
  linuxBuildFolder
];

// packaging
var windowsPackageFolder = artifactsFolder + "/Windows_Package";
var macOSXPackageFolder = artifactsFolder + "/MacOSX_Package";

var allPackageFolders = [
  windowsPackageFolder,
  macOSXPackageFolder
];

function getAtomicDevBuildDefine() {

  return "-DATOMIC_DEV_BUILD=" + (cmakeDevBuild ? "1" : "0");

}

function testRemoveDir(path) {

  if (fs.existsSync(path)) {
    jake.rmRf(path);
  }

  if (fs.existsSync(path)) {

    throw "Unable to remove path: " + path;

  }
}

namespace('clean', function() {

  task('all', {async:true}, function() {

    for (var i in allBuildFolders) {
      testRemoveDir(allBuildFolders[i]);
    }

    for (var i in allPackageFolders) {
      testRemoveDir(allPackageFolders[i]);
    }

    complete();

  });
});

namespace('build', function() {

  task('macosx', {async:true}, function() {

    if (!fs.existsSync(macOSXBuildFolder)) {
      jake.mkdirP(macOSXBuildFolder);
    }

    var cmds = [
      'cmake ../../ -G Xcode ' + getAtomicDevBuildDefine(),
      'xcodebuild -configuration Release'
    ]

    process.chdir(macOSXBuildFolder);

    jake.exec(cmds, function() {
      console.log("Built MacOSX");
      complete();
    }, {
      printStdout: true
    });

  });

});

namespace('package', function() {

  cmakeDevBuild = false;

  task('macosx', ['clean:all', 'build:macosx'], function() {

    jake.mkdirP(macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");

    var editorAppSourceFolder = macOSXBuildFolder + "/Source/AtomicEditor/Release/AtomicEditor.app";

    // copy app (using cp here as jake.cpR seems to strip executable flag)
    jake.exec("cp -r " + editorAppSourceFolder + " " + macOSXPackageFolder, {
      printStdout: true
    });

    // copy resources

    jake.cpR(jakeRoot + "/Resources/CoreData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");
    jake.cpR(jakeRoot + "/Resources/EditorData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");
    jake.cpR(jakeRoot + "/Resources/PlayerData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");
    jake.cpR(jakeRoot + "/Script", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");
    jake.cpR(jakeRoot + "/Data/AtomicEditor/ProjectTemplates", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");

  });

});
