// patch in our local node_modules
process.env.NODE_PATH = __dirname + "/Build/node_modules";
require('module').Module._initPaths();

var fs = require('fs-extra');
var path = require('path');
var os = require('os');

var host = os.platform();
var jakeRoot = __dirname;

var artifactsFolder = jakeRoot + "/Artifacts";

// cmake
var cmakeDevBuild = false;

var includeDeployments = true;
var deployments = {
  osx: true,
  android: true,
  windows: true,
  ios: true,
  web: true
}

// build folder
var windowsBuildFolder = artifactsFolder + "/Windows_Build";
var macOSXBuildFolder = artifactsFolder + "/MacOSX_Build";
var androidBuildFolder = artifactsFolder + "/Android_Build";
var iosBuildFolder = artifactsFolder + "/IOS_Build";
var iosDeployBuildFolder = artifactsFolder + "/ios-deploy";
var webBuildFolder = artifactsFolder + "/Web_Build";
var linuxBuildFolder = artifactsFolder + "/Linux_Build";

// binaries

if (host == "darwin") {
  var atomicToolBinary = macOSXBuildFolder + "/Source/AtomicTool/Release/AtomicTool"
  var atomicEditorBinary = macOSXBuildFolder + "/Source/AtomicEditor/Release/AtomicEditor.app/Contents/MacOS/AtomicEditor"
  var atomicPlayerBinary = macOSXBuildFolder + "/Source/AtomicPlayer/Application/Release/AtomicPlayer.app/Contents/MacOS/AtomicPlayer"
} else if (host == "win32") {
  var atomicToolBinary = windowsBuildFolder + "/Source/AtomicTool/Release/AtomicTool.exe"
  var atomicEditorBinary = windowsBuildFolder + "/Source/AtomicEditor/Release/AtomicEditor.exe"
  var atomicPlayerBinary = windowsBuildFolder + "/Source/AtomicPlayer/Application/Release/AtomicPlayer.exe"
}

// source files
var androidPlayerBinary = androidBuildFolder + "/Source/AtomicPlayer/Application/libAtomicPlayer.so";
var iosPlayerBinary = iosBuildFolder + "/Source/AtomicPlayer/Application/Release-iphoneos/AtomicPlayer.app/AtomicPlayer"
var webPlayerBinary = webBuildFolder + "/Source/AtomicPlayer/Application/AtomicPlayer.js"
var webPlayerMemFile = webBuildFolder + "/Source/AtomicPlayer/Application/AtomicPlayer.html.mem"

var allBuildFolders = [
  windowsBuildFolder,
  macOSXBuildFolder,
  androidBuildFolder,
  iosBuildFolder,
  iosDeployBuildFolder,
  webBuildFolder,
  linuxBuildFolder
];

// packaging
var platformBinariesFolder = artifactsFolder + "/Platform_Binaries";
var windowsPackageFolder = artifactsFolder + "/Windows_Package";
var macOSXPackageFolder = artifactsFolder + "/MacOSX_Package";

var allPackageFolders = [
  windowsPackageFolder,
  macOSXPackageFolder,
  platformBinariesFolder
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

  task('all', {
    async: true
  }, function() {

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

  task('macosx_atomictool', {
    async: true
  }, function() {

    if (!fs.existsSync(macOSXBuildFolder)) {
      jake.mkdirP(macOSXBuildFolder);
    }

    var cmds = [
      'cmake ../../ -G Xcode ' + getAtomicDevBuildDefine(),
      'xcodebuild -target AtomicTool -configuration Release'
    ]

    process.chdir(macOSXBuildFolder);

    jake.exec(cmds, function() {

      fs.copySync(atomicToolBinary, platformBinariesFolder + "/MacOSX/" + path.basename(atomicToolBinary));
      console.log("Built MacOSX AtomicTool");
      complete();
    }, {
      printStdout: true
    });




  }); // end build:macosx_atomictool

  var deps = [];

  if (includeDeployments) {

    if (host == 'darwin') {

      deps = ['macosx_atomictool'];

      if (deployments.ios)
        deps.push("build:ios");

      if (deployments.android)
        deps.push("build:android");

      if (deployments.web)
        deps.push("build:web");

    }

  }

  task('install_deployments', {
    async: true
  }, function() {

    console.log("Installing Deployments");

    // deployment root for local dev builds
    var deployRoot = jakeRoot + "/Data/AtomicEditor/Deployment";

    if (host == "darwin") {

      if (deployments.osx)
        fs.copySync(platformBinariesFolder + "/MacOSX/AtomicPlayer", deployRoot + "/MacOS/AtomicPlayer.app/Contents/MacOS/AtomicPlayer");

      if (deployments.android)
        fs.copySync(platformBinariesFolder + "/Android/libAtomicPlayer.so", deployRoot + "/Android/libs/armeabi-v7a/libAtomicPlayer.so");

      if (deployments.ios) {
        fs.copySync(artifactsFolder + "/ios-deploy/ios-deploy/ios-deploy", deployRoot + "/IOS/ios-deploy/ios-deploy");
        fs.copySync(platformBinariesFolder + "/IOS/AtomicPlayer", deployRoot + "/IOS/AtomicPlayer.app/AtomicPlayer");
      }

      if (deployments.web) {
        fs.copySync(platformBinariesFolder + "/Web/AtomicPlayer.js", deployRoot + "/Web/AtomicPlayer.js");
        fs.copySync(platformBinariesFolder + "/Web/AtomicPlayer.html.mem", deployRoot + "/Web/AtomicPlayer.html.mem");
      }

    } else if (host == "win32") {

      if (deployments.windows) {
        fs.copySync(platformBinariesFolder + "/Win32/AtomicPlayer.exe", deployRoot + "/Windows/x86/AtomicPlayer.exe");
      }

    }

    complete();

  });

  task('ios_deploy', {
    async: true
  }, function() {

    if (!fs.existsSync(iosDeployBuildFolder)) {
      jake.mkdirP(iosDeployBuildFolder);
    }

    process.chdir(iosDeployBuildFolder);

    if (!fs.existsSync(iosDeployBuildFolder + "/ios-deploy")) {
      jake.exec("git clone https://github.com/AtomicGameEngine/ios-deploy && cd ios-deploy && make ios-deploy");
    }

    complete();

  });


  task('macosx', deps, {
    async: true
  }, function() {

    if (!fs.existsSync(macOSXBuildFolder)) {
      jake.mkdirP(macOSXBuildFolder);
    }

    var cmds = [
      'cmake ../../ -G Xcode ' + getAtomicDevBuildDefine(),
      'xcodebuild -configuration Release'
    ]

    process.chdir(macOSXBuildFolder);

    jake.exec(cmds, function() {
      fs.copySync(atomicEditorBinary, platformBinariesFolder + "/MacOSX/" + path.basename(atomicEditorBinary));
      fs.copySync(atomicPlayerBinary, platformBinariesFolder + "/MacOSX/" + path.basename(atomicPlayerBinary));

      if (includeDeployments) {
        jake.Task['build:install_deployments'].invoke();
      }

      console.log("Built MacOSX");
      complete();
    }, {
      printStdout: true
    });

  }); // end build:macosx

  task('windows', {
    async: true
  }, function() {

    if (!fs.existsSync(windowsBuildFolder)) {
      jake.mkdirP(windowsBuildFolder);
    }

    process.chdir(windowsBuildFolder);

    jake.exec(jakeRoot + "/Build/Windows/Compile.bat", function() {

      fs.copySync(atomicEditorBinary, platformBinariesFolder + "/Win32/" + path.basename(atomicEditorBinary));
      fs.copySync(atomicPlayerBinary, platformBinariesFolder + "/Win32/" + path.basename(atomicPlayerBinary));

      if (includeDeployments) {
        jake.Task['build:install_deployments'].invoke();
      }

      complete();
    }, {
      printStdout: true
    });

  });

  task('android', ['macosx_atomictool'], {
    async: true
  }, function() {

    if (!fs.existsSync(androidBuildFolder)) {
      jake.mkdirP(androidBuildFolder);
    }

    process.chdir(androidBuildFolder);

    var cmds = [
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/Atomic/ ANDROID",
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/AtomicPlayer/ ANDROID",
      "cmake -DCMAKE_TOOLCHAIN_FILE=" + jakeRoot + "/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../",
      "make -j4"
    ]

    jake.exec(cmds, function() {
      fs.copySync(androidPlayerBinary, platformBinariesFolder + "/Android/" + path.basename(androidPlayerBinary));
      console.log("Built Android Player");
      complete();
    }, {
      printStdout: true
    });

  });

  task('ios', ['macosx_atomictool', 'ios_deploy'], {
    async: true
  }, function() {

    if (!fs.existsSync(iosBuildFolder)) {
      jake.mkdirP(iosBuildFolder);
    }

    process.chdir(iosBuildFolder);

    var cmds = [
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/Atomic/ IOS",
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/AtomicPlayer/ IOS",
      "cmake -DIOS=1 -G Xcode ../../",
      "security -v list-keychains -d system -s /Users/jenkins/Library/Keychains/codesign.keychain", // TODO: only if jenkins
      "security -v unlock-keychain /Users/jenkins/Library/Keychains/codesign.keychain", // TODO: only if jenkins
      "xcodebuild -configuration Release"
    ]

    jake.exec(cmds, function() {
      fs.copySync(iosPlayerBinary, platformBinariesFolder + "/IOS/" + path.basename(iosPlayerBinary));
      console.log("Built iOS Player");
      complete();
    }, {
      printStdout: true
    });

  });


  task('web', ['macosx_atomictool'], {
    async: true
  }, function() {

    if (!fs.existsSync(webBuildFolder)) {
      jake.mkdirP(webBuildFolder);
    }

    process.chdir(webBuildFolder);

    var cmds = [
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/Atomic/ WEB",
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/AtomicPlayer/ WEB",
      "cmake -DEMSCRIPTEN=1 -DATOMIC_BUILD_2D=1 -DCMAKE_TOOLCHAIN_FILE=" + jakeRoot + "/CMake/Toolchains/emscripten.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../",
      "make -j4",
      "cd ./Source/AtomicPlayer/Application && mv ./AtomicPlayer ./AtomicPlayer.bc && " +
      // -s ERROR_ON_UNDEFINED_SYMBOLS=1 (disabled for pthread errors currently on incoming)
      "emcc -O3 -s USE_PTHREADS=0 -s ASM_JS=1 -s VERBOSE=0 -s USE_SDL=2 -s TOTAL_MEMORY=134217728 -s AGGRESSIVE_VARIABLE_ELIMINATION=1 -s NO_EXIT_RUNTIME=1 ./AtomicPlayer.bc -o  ./AtomicPlayer.html"

    ]

    jake.exec(cmds, function() {
      fs.copySync(webPlayerBinary, platformBinariesFolder + "/Web/" + path.basename(webPlayerBinary));
      fs.copySync(webPlayerMemFile, platformBinariesFolder + "/Web/" + path.basename(webPlayerMemFile));
      console.log("Built Web Player");
      complete();
    }, {
      printStdout: true
    });

  });


}); // end build namespace

namespace('package', function() {

  task('macosx', ['build:ios'], function() {

    jake.mkdirP(macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");

    var editorAppSourceFolder = macOSXBuildFolder + "/Source/AtomicEditor/Release/AtomicEditor.app";

    fs.copySync(editorAppSourceFolder, macOSXPackageFolder + "/AtomicEditor.app");

    // copy resources

    fs.copySync(jakeRoot + "/Resources/CoreData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/CoreData");
    fs.copySync(jakeRoot + "/Resources/EditorData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/EditorData");
    fs.copySync(jakeRoot + "/Resources/PlayerData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/PlayerData");
    fs.copySync(jakeRoot + "/Data/AtomicEditor/", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/ToolData/");

  });

  task('windows', ['clean:all', 'build:windows'], function() {

    console.log("Packaging Windows Editor");

    jake.mkdirP(windowsPackageFolder + "/Resources");

    fs.copySync(windowsBuildFolder + "/Source/AtomicEditor/Release/AtomicEditor.exe", windowsPackageFolder + "/AtomicEditor.exe")

    // 32 bit build for packaging!

    fs.copySync(jakeRoot + "/Build/Windows/Binaries/x86/D3DCompiler_47.dll", windowsPackageFolder + "/D3DCompiler_47.dll")

    // copy resources

    fs.copySync(jakeRoot + "/Resources/CoreData", windowsPackageFolder + "/Resources/CoreData");
    fs.copySync(jakeRoot + "/Resources/EditorData", windowsPackageFolder + "/Resources/EditorData");
    fs.copySync(jakeRoot + "/Resources/PlayerData", windowsPackageFolder + "/Resources/PlayerData");
    fs.copySync(jakeRoot + "/Data/AtomicEditor/", windowsPackageFolder + "/Resources/ToolData");

    // 32 bit!
    fs.copySync(jakeRoot + "/Build/Windows/Binaries/x86/D3DCompiler_47.dll", windowsPackageFolder + "/Resources/ToolData/Deployment/Windows/x86/D3DCompiler_47.dll");

  });


});
