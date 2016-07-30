var fs = require('fs-extra');
var bcommon = require("./BuildCommon");
var host = require("./Host");

var buildDir = bcommon.artifactsRoot + "Build/EditorData/";
var jsDocFolder = bcommon.artifactsRoot + "Build/JSDoc/";
var atomicRoot = bcommon.atomicRoot;
var atomicTool = host.getAtomicToolBinary();
var glob = require("glob");
var Tslint = require("tslint");

namespace('build', function() {

  task('genscriptbindings', {
    async: true
  }, function() {

    bcommon.cleanCreateDir(buildDir);
    bcommon.cleanCreateDir(jsDocFolder);

    var bindCmd = atomicTool + " bind \"" + atomicRoot + "\" ";

    var cmds = [
      bindCmd + "Script/Packages/Atomic/ WINDOWS",
      bindCmd + "Script/Packages/AtomicPlayer/ WINDOWS",
      bindCmd + "Script/Packages/ToolCore/ WINDOWS",
      bindCmd + "Script/Packages/Editor/ WINDOWS",
      bindCmd + "Script/Packages/WebView/ WINDOWS",
      bindCmd + "Script/Packages/AtomicNETScript/ WINDOWS",
      bindCmd + "Script/Packages/AtomicApp/ WINDOWS"
    ];

    jake.exec(cmds, function() {

      console.log("Built Script Bindings");

      complete();

    }, {
      printStdout: true
    });

  });

  task('gendocs', ["build:genscriptbindings"], {
    async: true
  }, function() {

    console.log("Generating Examples & JSDocs");

    fs.copySync(atomicRoot + "Build/Docs/Readme.md", jsDocFolder + "Readme.md");
    fs.copySync(atomicRoot + "Build/Docs/atomic-theme", jsDocFolder + "atomic-theme");

    cmds = [
      "git clone https://github.com/AtomicGameEngine/AtomicExamples " + buildDir + "AtomicExamples && rm -rf " + buildDir + "AtomicExamples/.git",
      "cd " + jsDocFolder + " && npm install typedoc@0.3.12",
      "cd " + jsDocFolder + " && ./node_modules/.bin/typedoc --out out ../../../Script/TypeScript/dist/Atomic.d.ts --module commonjs --includeDeclarations --mode file --theme atomic-theme --name 'Atomic Game Engine' --readme ./Readme.md",
    ];

    jake.exec(cmds, function() {

      fs.copySync(jsDocFolder + "out", buildDir + "Docs");
      complete();

    }, {
      printStdout: true
    });


  });

  task('ios_deploy', {
    async: true
  }, function() {

    var iosDeploybuildDir = bcommon.artifactsRoot + "Build/IOSDeploy/";

    bcommon.cleanCreateDir(iosDeploybuildDir);

    process.chdir(iosDeploybuildDir);

    jake.exec("git clone https://github.com/AtomicGameEngine/ios-deploy && cd ios-deploy && make ios-deploy",
      function() {

        complete();
      }, {
        printStdout: true
      });

  });

  // Linting task
  task('lint_typescript', {
      async: true
  }, function(fileMask, failOnError) {

    console.log("TSLINT: Linting files in " + fileMask);
    var lintConfig = JSON.parse(fs.readFileSync("./Script/tslint.json"));
    var options = {
        configuration: lintConfig,
        formatter: "prose"
    };

    // lint
    // Since TSLint does not yet support recursively searching for files, then we need to
    // create a command per file.  The main issue with this is that it will abort on the first error instead
    // of listing out all lint errors
    glob(fileMask, function(err, results) {
      var lintErrors = [];
      results.forEach(function(filename) {

        var contents = fs.readFileSync(filename, "utf8");

        var ll = new Tslint(filename, contents, options);
        var result = ll.lint();
        if (result.failureCount > 0) {
            lintErrors.push(result.output);
        }
      });
      if (lintErrors.length > 0) {
          console.warn("TSLINT: WARNING - Lint errors detected");
          console.warn(lintErrors.join(''));
          if (failOnError) {
              fail("TSLint errors detected");
          }
      }
      complete();
    });
  });

  task('compileeditorscripts', ["build:genscriptbindings"],{
    async: true
  }, function() {

    console.log("Compiling Editor Scripts");

    process.chdir(atomicRoot);

    var tsc = "./Build/node_modules/typescript/lib/tsc";
    var dtsGenerator = "./Build/node_modules/dts-generator/bin/dts-generator";

    cmds = [
      atomicRoot + "Build/Mac/node/node " + tsc + " -p ./Script",
      atomicRoot + "Build/Mac/node/node " + tsc + " -p ./Script/AtomicWebViewEditor",
      atomicRoot + "Build/Mac/node/node " + dtsGenerator + " --name Atomic --project ./Script/TypeScript --out ./Script/TypeScript/dist/Atomic.d.ts"
    ];

      // will be copied when editor resources are copied

    var lintTask = jake.Task['build:lint_typescript'];

    lintTask.addListener('complete', function () {
      console.log("\n\nLint: Typescript linting complete.\n\n");
      jake.exec(cmds, function() {

          // copy some external dependencies into the editor modules directory
         var editorModulesDir = "./Artifacts/Build/Resources/EditorData/AtomicEditor/EditorScripts/AtomicEditor/modules";
         var webeditorModulesDir = "./Data/AtomicEditor/CodeEditor/source/editorCore/modules";
         var nodeModulesDir = "./Build/node_modules";
         fs.mkdirsSync(editorModulesDir);
         // TypeScript
         fs.copySync(nodeModulesDir + "/typescript/lib/typescript.js", webeditorModulesDir + "/typescript.js")

         // copy lib.core.d.ts into the tool data directory
         fs.mkdirsSync("./Artifacts/Build/Resources/EditorData/AtomicEditor/EditorScripts/AtomicEditor/TypeScriptSupport");
         fs.copySync("./Build/node_modules/typescript/lib/lib.core.d.ts","./Data/AtomicEditor/TypeScriptSupport/lib.core.d.ts")

         // copy the combined Atomic.d.ts to the tool data directory
         fs.copySync("./Script/TypeScript/dist/Atomic.d.ts","./Data/AtomicEditor/TypeScriptSupport/Atomic.d.ts")
         complete();

      }, {
        printStdout: true
      });
    });

    lintTask.invoke("{./Script/AtomicEditor/**/*.ts,./Script/AtomicWebViewEditor/**/*.ts}", false);
  });

  task('geneditordata', ["build:compileeditorscripts", "build:ios_deploy", "build:gendocs"], {
    async: true
  }, function() {

    // Mac App

    fs.copySync(atomicRoot + "Build/CIScripts/Mac/PlayerApp/",
      buildDir + "MacApps/PlayerApp/");

    // Editor Binaries

    fs.copySync(bcommon.artifactsRoot + "Build/Mac/Bin/AtomicEditor.zip",
      buildDir + "EditorBinaries/Mac/AtomicEditor.zip");

    fs.copySync(bcommon.artifactsRoot + "Build/Windows/Bin/AtomicEditor.exe",
      buildDir + "EditorBinaries/Windows/AtomicEditor.exe");

    fs.copySync(bcommon.artifactsRoot + "Build/Windows/Bin/D3DCompiler_47.dll",
      buildDir + "EditorBinaries/Windows/D3DCompiler_47.dll");

    // Resources

    fs.copySync(atomicRoot + "Resources/CoreData", buildDir + "Resources/CoreData");
    fs.copySync(atomicRoot + "Resources/EditorData", buildDir + "Resources/EditorData");
    fs.copySync(atomicRoot + "Resources/PlayerData", buildDir + "Resources/PlayerData");
    fs.copySync(atomicRoot + "/Data/AtomicEditor", buildDir + "Resources/ToolData");

    fs.copySync(atomicRoot + "Artifacts/Build/Resources/EditorData/AtomicEditor/EditorScripts",
      buildDir + "Resources/EditorData/AtomicEditor/EditorScripts");

    // root deployment
    var deployRoot = buildDir + "Resources/ToolData/Deployment/";

    fs.copySync(atomicRoot + "/Data/AtomicEditor/Deployment/", deployRoot);

    // Android
    fs.copySync(bcommon.artifactsRoot + "Build/Android/Bin/libAtomicPlayer.so",
      deployRoot + "Android/libs/armeabi-v7a/libAtomicPlayer.so");

    // Mac
    fs.copySync(bcommon.artifactsRoot + "Build/Mac/Bin/AtomicPlayer",
      deployRoot + "MacOS/AtomicPlayer.app/Contents/MacOS/AtomicPlayer");

    // IOS
    fs.copySync(bcommon.artifactsRoot + "Build/IOSDeploy/ios-deploy/ios-deploy",
      deployRoot + "IOS/ios-deploy/ios-deploy");
    fs.copySync(bcommon.artifactsRoot + "Build/IOS/Bin/AtomicPlayer",
      deployRoot + "IOS/AtomicPlayer.app/AtomicPlayer");

    // Web
    fs.copySync(bcommon.artifactsRoot + "Build/Web/Bin/AtomicPlayer.js",
      deployRoot + "Web/AtomicPlayer.js");
    fs.copySync(bcommon.artifactsRoot + "Build/Web/Bin/AtomicPlayer.html.mem",
      deployRoot + "Web/AtomicPlayer.html.mem");

    // Windows
    fs.copySync(bcommon.artifactsRoot + "Build/Windows/Bin/AtomicPlayer.exe",
      deployRoot + "Windows/x64/AtomicPlayer.exe");
    fs.copySync(bcommon.artifactsRoot + "Build/Windows/Bin/D3DCompiler_47.dll",
      deployRoot + "Windows/x64/D3DCompiler_47.dll");


    complete();

    process.chdir(buildDir);

    var cmds = ["zip -r -X ./EditorData.zip ./"];

    jake.exec(cmds,
      function() {
        complete();
      }, {
        printStdout: true
      });

  });

}); // end of build namespace
