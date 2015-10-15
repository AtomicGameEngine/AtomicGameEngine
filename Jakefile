// patch in our local node_modules
process.env.NODE_PATH = __dirname + "/Build/node_modules";
require('module').Module._initPaths();

var fs = require('fs-extra');
var path = require('path');
var os = require('os');

var host = os.platform();
var jakeRoot = __dirname;
var jenkinsBuild = process.env.ATOMIC_JENKINS_BUILD == 1
var generateJSDoc = true;

var artifactsFolder = jakeRoot + "/Artifacts";

// cmake
var cmakeDevBuild = false;

var includeDeployments = jenkinsBuild;

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
var jsDocFolder = artifactsFolder + "/JSDoc";

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
  linuxBuildFolder,
  jsDocFolder
];

// packaging
var platformBinariesFolder = artifactsFolder + "/Platform_Binaries";
var windowsPackageFolder = artifactsFolder + "/Windows_Package";
var macOSXPackageFolder = artifactsFolder + "/MacOSX_Package";
var distFolder = artifactsFolder + "/Dist";

var allPackageFolders = [
  windowsPackageFolder,
  macOSXPackageFolder,
  platformBinariesFolder,
  distFolder
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

  task('atomictool', {
    async: true
  }, function() {

    if (host == "darwin") {

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

  } else {

    if (!fs.existsSync(windowsBuildFolder)) {
      jake.mkdirP(windowsBuildFolder);
    }

    process.chdir(windowsBuildFolder);

    jake.exec(jakeRoot + "/Build/Windows/CompileAtomicTool.bat", function() {

      fs.copySync(atomicToolBinary, platformBinariesFolder + "/Win32/" + path.basename(atomicToolBinary));
      console.log("Built Windows AtomicTool");
      complete();

    }, {
      printStdout: true
    });

  }

  }); // end build:atomictool

  var deps = [];

  if (includeDeployments) {

    if (host == 'darwin') {

      deps = ['atomictool'];

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

  task('android', ['atomictool'], {
    async: true
  }, function() {

    if (!fs.existsSync(androidBuildFolder)) {
      jake.mkdirP(androidBuildFolder);
    }

    process.chdir(androidBuildFolder);

    var cmds = [
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/Atomic/ ANDROID",
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/AtomicPlayer/ ANDROID"
    ];

    if (host == "darwin") {

      cmds.push("cmake -G \"Unix Makefiles\" -DCMAKE_TOOLCHAIN_FILE=" + jakeRoot + "/CMake/Toolchains/android.toolchain.cmake -DCMAKE_BUILD_TYPE=Release ../../");
      cmds.push("make -j4");

      jake.exec(cmds, function() {
        fs.copySync(androidPlayerBinary, platformBinariesFolder + "/Android/" + path.basename(androidPlayerBinary));
        console.log("Built Android Player");
        complete();
      }, {
        printStdout: true
      });
    } else {

      cmds.push(jakeRoot + "/Build/Windows/CompileAndroid.bat");

      jake.exec(cmds, function() {

        fs.copySync(androidPlayerBinary, platformBinariesFolder + "/Android/" + path.basename(androidPlayerBinary));

        // sneak this in here, need to sort deployments for local dev builds
        var deployRoot = jakeRoot + "/Data/AtomicEditor/Deployment";
        fs.copySync(androidPlayerBinary, deployRoot + "/Android/libs/armeabi-v7a/libAtomicPlayer.so");

        console.log("Built Android Player");
        complete();

      }, {
        printStdout: true
      });

    }

  });

  task('ios', ['atomictool', 'ios_deploy'], {
    async: true
  }, function() {

    if (!fs.existsSync(iosBuildFolder)) {
      jake.mkdirP(iosBuildFolder);
    }

    process.chdir(iosBuildFolder);

    var cmds = [
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/Atomic/ IOS",
      atomicToolBinary + " bind " + jakeRoot + " Script/Packages/AtomicPlayer/ IOS",
      "cmake -DIOS=1 -G Xcode ../../"
    ]

    if (jenkinsBuild) {

      cmds.push("security -v list-keychains -d system -s /Users/jenkins/Library/Keychains/codesign.keychain");
      cmds.push("security -v unlock-keychain /Users/jenkins/Library/Keychains/codesign.keychain");

    }

    cmds.push("xcodebuild -configuration Release");

    jake.exec(cmds, function() {
      fs.copySync(iosPlayerBinary, platformBinariesFolder + "/IOS/" + path.basename(iosPlayerBinary));
      console.log("Built iOS Player");
      complete();
    }, {
      printStdout: true
    });

  });


  task('web', ['atomictool'], {
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

  task('genjsdocs', {
    async: true
  }, function() {

    if (!jenkinsBuild || !generateJSDoc) {
      complete();
      return;
    }

    console.log("Generating JSDocs");

    fs.copySync(jakeRoot + "/Docs/Readme.md", jsDocFolder + "/Readme.md");
    fs.copySync(jakeRoot + "/Docs/jsdoc.conf", jsDocFolder + "/jsdoc.conf");

    cmds = ["cd " + jsDocFolder + " && npm install git+https://github.com/jsdoc3/jsdoc",
      "cd " + jsDocFolder + " && git clone https://github.com/AtomicGameEngine/jaguarjs-jsdoc && cd jaguarjs-jsdoc && git checkout atomic_master",
      "cd " + jsDocFolder + " && ./node_modules/.bin/jsdoc ./Atomic.js -t ./jaguarjs-jsdoc/ -c ./jsdoc.conf Readme.md",
    ];

    jake.exec(cmds, function() {

      complete();

    }, {
      printStdout: true
    });

  });

  task('macosx', ['clean:all', 'build:macosx', 'package:genjsdocs'], function() {

    if (!fs.existsSync(distFolder)) {
      jake.mkdirP(distFolder);
    }

    jake.mkdirP(macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/");

    var editorAppSourceFolder = macOSXBuildFolder + "/Source/AtomicEditor/Release/AtomicEditor.app";

    fs.copySync(editorAppSourceFolder, macOSXPackageFolder + "/AtomicEditor.app");

    // copy resources

    fs.copySync(jakeRoot + "/Resources/CoreData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/CoreData");
    fs.copySync(jakeRoot + "/Resources/EditorData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/EditorData");
    fs.copySync(jakeRoot + "/Resources/PlayerData", macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/PlayerData");

    var toolDataDir = macOSXPackageFolder + "/AtomicEditor.app/Contents/Resources/ToolData/";

    fs.copySync(jakeRoot + "/Data/AtomicEditor/", toolDataDir);

    if (jenkinsBuild) {

      console.log("Exporting AtomicExamples Repo (master) & Generating Dist");

      if (generateJSDoc) {

        fs.copySync(jsDocFolder + "/out", toolDataDir + "Docs/JSDocs");

      }

      cmds = ["git clone https://github.com/AtomicGameEngine/AtomicExamples " + toolDataDir + "AtomicExamples && rm -rf " + toolDataDir + "AtomicExamples/.git",
        "cd " + macOSXPackageFolder + " && zip -r -X " + distFolder + "/AtomicEditor_MacOSX.zip ./AtomicEditor.app"
      ];

      jake.exec(cmds, function() {});
    }

  });

  task('windows', ['clean:all','build:windows'], function() {

    if (!fs.existsSync(distFolder)) {
      jake.mkdirP(distFolder);
    }

    console.log("Packaging Windows Editor");

    jake.mkdirP(windowsPackageFolder + "/Resources");

    fs.copySync(windowsBuildFolder + "/Source/AtomicEditor/Release/AtomicEditor.exe", windowsPackageFolder + "/AtomicEditor.exe")

    // 64 bit build for packaging!

    fs.copySync(jakeRoot + "/Build/Windows/Binaries/x64/D3DCompiler_47.dll", windowsPackageFolder + "/D3DCompiler_47.dll")

    // copy resources

    fs.copySync(jakeRoot + "/Resources/CoreData", windowsPackageFolder + "/Resources/CoreData");
    fs.copySync(jakeRoot + "/Resources/EditorData", windowsPackageFolder + "/Resources/EditorData");
    fs.copySync(jakeRoot + "/Resources/PlayerData", windowsPackageFolder + "/Resources/PlayerData");
    fs.copySync(jakeRoot + "/Data/AtomicEditor/", windowsPackageFolder + "/Resources/ToolData");

    // AtomicNET
    fs.copySync(jakeRoot + "/Submodules/CoreCLR/Windows/Debug", windowsPackageFolder + "/Resources/AtomicNET/Windows/");
    fs.copySync(jakeRoot + "/Artifacts/AtomicNET", windowsPackageFolder + "/Resources/AtomicNET/Windows/Atomic");

    fs.copySync(jakeRoot + "/Build/Windows/Binaries/x86/D3DCompiler_47.dll", windowsPackageFolder + "/Resources/ToolData/Deployment/Windows/x86/D3DCompiler_47.dll");
    fs.copySync(jakeRoot + "/Build/Windows/Binaries/x64/D3DCompiler_47.dll", windowsPackageFolder + "/Resources/ToolData/Deployment/Windows/x64/D3DCompiler_47.dll");

    if (jenkinsBuild) {

      cmds = [jakeRoot + "/Build/Windows/7z/7z.exe a -tzip " + distFolder + "/AtomicEditor_Windows.zip " + windowsPackageFolder]

      jake.exec(cmds, function() {});

    }

  });

  // We have the windows and mac editors builds, now we need to merge them
  task('merge_editor_data', [], function() {

    var mergeFolder = distFolder + "/EditorMerge";

    if (!fs.existsSync(mergeFolder)) {
      jake.mkdirP(mergeFolder);
    }

    if (!fs.existsSync(mergeFolder + "/snapshots")) {
      jake.mkdirP(mergeFolder + "/snapshots");
    }

    fs.copySync(distFolder + "/AtomicEditor_MacOSX.zip", mergeFolder + "/AtomicEditor_MacOSX.zip");
    fs.copySync(windowsPackageFolder + "/Artifacts/Dist/AtomicEditor_Windows.zip", mergeFolder + "/AtomicEditor_Windows.zip");

    process.chdir(mergeFolder);

    cmds = ["unzip ./AtomicEditor_MacOSX.zip && unzip ./AtomicEditor_Windows.zip"]

    jake.exec(cmds, function() {

      var winRoot = mergeFolder + "/Windows_Package/Resources";
      var macRoot = mergeFolder + "/AtomicEditor.app/Contents/Resources";

      var filenames = [
        "/ToolData/Deployment/Android/libs/armeabi-v7a/libAtomicPlayer.so",
        "/ToolData/Deployment/MacOS/AtomicPlayer.app",
        "/ToolData/Deployment/Web/AtomicPlayer.js",
        "/ToolData/Deployment/Web/AtomicPlayer.html.mem",
        "/ToolData/AtomicExamples",
        "/ToolData/Docs",
      ];

      // Mac Editor -> Windows Editor
      for (var i in filenames) {
          fs.copySync(macRoot + filenames[i], winRoot + filenames[i]);
      }

      // Windows Editor ->Mac Editor
      filenames = ["/ToolData/Deployment/Windows/x86/D3DCompiler_47.dll",
        "/ToolData/Deployment/Windows/x86/AtomicPlayer.exe",
      ];

      for (var i in filenames) {
          fs.copySync(winRoot + filenames[i], macRoot + filenames[i]);
      }

      zipcmds = [
        "rev=`git rev-parse HEAD` && zip -r -X ./snapshots/AtomicEditor_MacOSX_DevSnapshot_$rev.zip ./AtomicEditor.app",
        "rev=`git rev-parse HEAD` && zip -r -X ./snapshots/AtomicEditor_Win32_DevSnapshot_$rev.zip ./Windows_Package",
        "cp ./snapshots/*.zip /Users/jenge/Share/Temp/AtomicEditor_DevSnapshots/"
      ]

      jake.exec(zipcmds, function() {});

    });

  });


});
