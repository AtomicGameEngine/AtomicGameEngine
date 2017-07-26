var fs = require('fs-extra');
var os = require('os');
var path = require("path");
var glob = require('glob');
var Tslint = require("tslint");
var fso = require('fs');
var spawnSync = require('child_process').spawnSync;

var host = require("./Host");
var config = require('./BuildConfig');

var atomicRoot = config.atomicRoot;
var jsDocFolder = config.artifactsRoot + "Build/JSDoc/";
var cppDocFolder = config.artifactsRoot + "Build/CPPDocs/";
var csharpDocFolder = config.artifactsRoot + "Build/CSharpDocs/";

namespace('build', function() {

    // Linting task
    task('lint_typescript', {
        async: true
    }, function(fileMask, failOnError) {

        console.log("TSLINT: Linting files in " + fileMask);
        var lintConfig = JSON.parse(fs.readFileSync("./Script/tslint.json"));
        var tslintConfig = {
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

                var ll = new Tslint(filename, contents, tslintConfig);
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

    // precreate script bindgs so they can be picked up by CMake
    task('precreateScriptBindings', {
        async: true
    }, function(clean) {

        if (clean === undefined) {
            clean = true;
        }
        console.log("Precreating script bindings");

        if (clean) {
            common.cleanCreateDir(common.getGenScriptRootDir())
        }

        common.createGenScriptFiles();

        complete();
    });

    function fileExists(filePath)
    {
        try
        {
            return fs.statSync(filePath).isFile();
        }
        catch (err)
        {
            return false;
        }
    }

    task('genAtomicNET', {
      async:true
    }, function(platform, configuration) {

      if (configuration != "Debug" && configuration != "Release")
        configuration = "Release";

      // Compile AtomicNET assemblies
      var cmds = [];

      cmds.push(host.atomicTool + " net compile " + atomicRoot + "Script/AtomicNET/AtomicNETProject.json -platform " + platform + " -config " + configuration);

      jake.exec(cmds, function() {

          complete();

      }, {
          printStdout: true
      });

    })

    task('genscripts', {
        async: true
    }, function(force) {

        // default to true
        if (force != "true" && force != "false") {
            force = "true";
        }

        var anyZero = false;
        if (force != "true") {

            var filenames = common.getGenScriptFilenames();
            for (var i in filenames) {

                if (!fileExists(filenames[i]))
                {
                    console.log("genscripts: file missing, regenerating script bindings: " + filenames[i]);
                    anyZero = true;
                    break;
                }

                var stats = fs.statSync(filenames[i]);
                if (stats["size"] == 0) {
                    console.log("genscripts: file zero size, regenerating script bindings: " + filenames[i]);
                    anyZero = true;
                    break;
                }
            }

            if (!anyZero)
                return;
        }


        process.chdir(atomicRoot);

        var modules = host.getScriptModules();
        var bindCmd = host.atomicTool + " bind \"" + atomicRoot + "\" ";
        var node = host.node;
        var tsc = "./Build/node_modules/typescript/lib/tsc";
        var tslint = "./Build/node_modules/tslint/lib/tslint-cli";
        var dtsGenerator = "./Build/node_modules/dts-generator/bin/dts-generator";

        var cmds = [];
        for (var pkgName in modules) {
            cmds.push(bindCmd + "Script/Packages/" + pkgName + "/");
        }

        if (node) {
            // compile
            cmds.push(node + " " + tsc + " -p ./Script");
            cmds.push(node + " " + tsc + " -p ./Script/AtomicWebViewEditor");

            // generate combined atomic.d.ts
            cmds.push(node + " " + dtsGenerator + " --name Atomic --project ./Script/TypeScript --out ./Script/TypeScript/dist/Atomic.d.ts");

            var lintTask = jake.Task['build:lint_typescript'];

            lintTask.addListener('complete', function () {

                console.log("\n\nLint: Typescript linting complete.\n\n");

                jake.exec(cmds, { printStdout : true, printStderr: true }, function() {

                    // copy some external dependencies into the editor modules directory
                    var editorModulesDir = "./Artifacts/Build/Resources/EditorData/AtomicEditor/EditorScripts/AtomicEditor/modules";
                    var webeditorModulesDir = "./Data/AtomicEditor/CodeEditor/source/editorCore/modules";
                    var nodeModulesDir = "./Build/node_modules";
                    fs.mkdirsSync(editorModulesDir);
                    // TypeScript
                    fs.copySync(nodeModulesDir + "/typescript/lib/typescript.js", webeditorModulesDir + "/typescript.js")

                    // copy lib.core.d.ts into the tool data directory
                    fs.mkdirsSync("./Artifacts/Build/Resources/EditorData/AtomicEditor/EditorScripts/AtomicEditor/TypeScriptSupport");
                    fs.copySync("./Build/node_modules/typescript/lib/lib.es5.d.ts","./Data/AtomicEditor/TypeScriptSupport/lib.es5.d.ts");

                    // copy the combined Atomic.d.ts to the tool data directory
                    fs.copySync("./Script/TypeScript/dist/Atomic.d.ts","./Data/AtomicEditor/TypeScriptSupport/Atomic.d.ts")

                    complete();

                }, {
                    printStdout: true
                });
            });

            lintTask.invoke("{./Script/AtomicEditor/**/*.ts,./Script/AtomicWebViewEditor/**/*.ts}", false);

        } else {
            throw new Error("Node not configured for this platform: " + os.platform());
        }


    });


  task('gendocs', {
    async: true
    }, function() {

    console.log( "Generating Docs..." );

    fs.copySync(atomicRoot + "Build/Docs/Readme.md", jsDocFolder + "Readme.md");
    fs.copySync(atomicRoot + "Build/Docs/atomic-theme", jsDocFolder + "atomic-theme");


    var typeDoc;
    if (os.platform() == "win32") {
        // uses system node for typedoc, which should have as require npm
        typeDoc = "node_modules\\.bin\\typedoc.cmd";
    }
    else
        typeDoc = host.node + " ./node_modules/.bin/typedoc";

    // tsdoc is having problems when name has spaces on Windows and Linux, tried quoting/escaping
    // what should happen here is instead of command line use a json config file (or maybe new version of tsdoc fixes this)
    var name = "Atomic-Game-Engine";

    cmds = [
      "cd " + jsDocFolder + " && echo {} > package.json", // newer versions of npm require package.json to be in the folder or else it searches up the heirarchy
      "cd " + jsDocFolder + " && npm install typedoc",
      "cd " + jsDocFolder + " && " + typeDoc + " --out out " + config.atomicRoot +
              "Script/TypeScript/dist/Atomic.d.ts --module commonjs --includeDeclarations --mode file --theme atomic-theme --name " +
              name + " --readme ./Readme.md"
    ];

    jake.exec(cmds, function() {

      common.cleanCreateDir( config.toolDataFolder + "Docs");

      fs.copySync(jsDocFolder + "out", config.toolDataFolder + "Docs/JSDocs");

      complete();

      console.log( "completed installing API documentation" );

    }, {

      printStdout: true

    });

  });

  task('gendoxygen', {
    async: true
    }, function() {

    console.log( "Generating C++ API Documentation..." );

    var cppDoc;
    if (os.platform() == "win32") {
        cppDoc = "doxygen";  // I dont know what to do here...
    }
    else {
        // use doxygen on path
        cppDoc = "doxygen";
    }

    cmds = [
      "cd " + atomicRoot + "Source && " + cppDoc + " " + atomicRoot + "Build/Docs/CPlusPlus/Doxyfile"
    ];

    jake.exec(cmds, function() {

      common.cleanCreateDir( config.toolDataFolder + "Docs/CPPDocs"); // clear destination

      fs.copySync(cppDocFolder, config.toolDataFolder + "Docs/CPPDocs"); // copy into release same place as JSDocs

      complete();

      console.log( "completed installing CPP API documentation" );

    }, {

      printStdout: true

    });

  });

  task('genmdoc', {
    async: true
    }, function() {

    console.log( "Generating C# API Documentation..." );

    // mdoc must be on path
    var mdoc = "mdoc";

    // clear destination
    common.cleanCreateDir( csharpDocFolder );

    cmds = [
      "cd " + csharpDocFolder + " && " + mdoc + " update -o docgen -i ../../AtomicNET/Release/Desktop/AtomicNET.xml ../../AtomicNET/Release/Desktop/AtomicNET.dll",
      "cd " + csharpDocFolder + " && " + mdoc + " export-html -o html docgen --template=../../../Build/Docs/CSharp/atomictemplate.xlst"
    ];

    jake.exec(cmds, function() {

      // clear destination
      common.cleanCreateDir( config.toolDataFolder + "Docs/CSharpDocs");

      // copy into release same place as JSDocs
      fs.copySync(csharpDocFolder, config.toolDataFolder + "Docs/CSharpDocs");

      complete();

      console.log( "completed installing C# API documentation" );

    }, {

      printStdout: true

    });

  });

  task('genexamples', {
    async: true
    }, function() {

    console.log( "Generating Examples..." );

    // TODO: support pulling examples from a specific branch/commit/etc
    var exampleSrc = atomicRoot + "Submodules/AtomicExamples/";
    var exampleDst = config.toolDataFolder + "AtomicExamples/";

    common.testRemoveDir( exampleDst);

    fs.copySync(exampleSrc, exampleDst);

    // remove the .git file/folder and save some space
    fs.removeSync( exampleDst + ".git" );

    complete();

  });



}); // end of build namespace
