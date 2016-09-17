var os = require('os');
var path = require('path');
var config = require("./BuildConfig");

// get the root folder
var atomicRoot = config.atomicRoot;
var nodeBinary;
switch(os.platform()) {
    case "win32":
    nodeBinary = atomicRoot + "Build\\Windows\\node\\node.exe";
    break;
    case "darwin":
    nodeBinary = atomicRoot + "Build/Mac/node/node";
    break;
    case "linux":
    nodeBinary = atomicRoot + "Build/Linux/node/node";
    break;
}


// patch in our local node_modules
var fs = require('fs-extra');

/// Returns a list of all script packages
function getScriptPackages() {

  var srcpath = atomicRoot + "Script/Packages/";

  return fs.readdirSync(srcpath).filter(function(file) {
    return fs.statSync(path.join(srcpath, file)).isDirectory() && fs.existsSync(srcpath + "/" + file + "/Package.json");
  });

}

// return an object with package name keys and module name lists as values
function getScriptModules() {

  modules = {};

  var packages = getScriptPackages();

  for (var i in packages) {

    var pkg = JSON.parse(fs.readFileSync(atomicRoot + "Script/Packages/" + packages[i] + "/Package.json"));

    for (var j in pkg.modules) {

      var moduleName = pkg.modules[j];

      if (!modules[pkg.name]) {
        modules[pkg.name] = {
          moduleNames: [],
          bindings: pkg.bindings
        };
      }

      modules[pkg.name].moduleNames.push(moduleName);
    }

  }

  return modules;

}

function getGenScriptRootDir() {

  return atomicRoot + "Artifacts/Build/Source/Generated/";

}

// Get a list of script source filenames
function getGenScriptFilenames() {

  var filenames = [];

  var scriptGenRoot = getGenScriptRootDir();

  var modules = getScriptModules();

  for (var pkgName in modules) {

    var module = modules[pkgName];

    // handle JS bindings
    if (!module.bindings || module.bindings.indexOf("JavaScript") != -1) {

      var jsPackageFolder = scriptGenRoot + "Javascript/Packages/" + pkgName + "/";

      // the JS package sources
      filenames.push(jsPackageFolder + "JSPackage" + pkgName + ".cpp");

      for (var i in module.moduleNames) {
        filenames.push(jsPackageFolder + "JSModule" + module.moduleNames[i] + ".cpp");
      }
    }

    // C#
    if (!module.bindings || module.bindings.indexOf("CSharp") != -1) {

      var csPackageFolder = scriptGenRoot + "CSharp/Packages/" + pkgName + "/";
      var csPackageNativeFolder = csPackageFolder + "Native/";
      var csPackageManagedFolder = csPackageFolder + "Managed/";

      // Native Package sources
      filenames.push(csPackageNativeFolder + "CSPackage" + pkgName + ".cpp");
      filenames.push(csPackageNativeFolder + "CSPackage" + pkgName + ".h");

      for (var i in module.moduleNames) {
        filenames.push(csPackageNativeFolder + "CSModule" + module.moduleNames[i] + ".cpp");
      }

    }

  }

  return filenames;

}

function createGenScriptFiles() {

  var scriptFiles = getGenScriptFilenames();

  for (var i in scriptFiles) {

    fs.ensureFileSync(scriptFiles[i]);

  }

}

function cleanCreateDir(directory) {

  testRemoveDir(directory);
  testCreateDir(directory);

}

function testCreateDir(directory) {

  if (fs.existsSync(directory)) {
    fail("Path already exists: " + directory);
  }

  jake.mkdirP(directory);

  if (!fs.existsSync(directory)) {
    fail("Unable to create path: " + directory);
  }
}

function setupDirs(clean, createDirs, removeDirs) {

    if (createDirs) {
        for (var i = 0; i < createDirs.length; i++) {

            var path = createDirs[i];
            if (!fs.existsSync(path) || clean) {
                cleanCreateDir(path);
            }
        }
    }

    if (removeDirs) {
        for (var i = 0; i < removeDirs.length; i++) {

            var path = removeDirs[i];
            if (fs.existsSync(path) && clean) {
                testRemoveDir(path);
            }
        }
    }

}

function testRemoveDir(path) {

  if (fs.existsSync(path)) {
    jake.rmRf(path);
  }

  if (fs.existsSync(path)) {
    fail("Unable to remove path: " + path);
  }
}

exports.atomicRoot = atomicRoot;
exports.artifactsRoot = atomicRoot + "Artifacts/";
exports.node = nodeBinary;
exports.cleanCreateDir = cleanCreateDir;
exports.testCreateDir = testCreateDir;
exports.testRemoveDir = testRemoveDir;
exports.getScriptPackages = getScriptPackages;
exports.getScriptModules = getScriptModules;
exports.getGenScriptFilenames = getGenScriptFilenames;
exports.createGenScriptFiles = createGenScriptFiles;
exports.getGenScriptRootDir = getGenScriptRootDir;
exports.setupDirs = setupDirs;
