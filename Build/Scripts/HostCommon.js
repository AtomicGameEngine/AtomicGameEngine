var os = require('os');
var path = require('path');

// get the root folder
var atomicRoot = path.resolve(__dirname, "../..") + "/";

// patch in our local node_modules
process.env.NODE_PATH = atomicRoot + "Build/node_modules/";
require('module').Module._initPaths();
var fs = require('fs-extra');

/// Returns a list of all script packages, regardless of platform
function getScriptPackages() {

  var srcpath = atomicRoot + "Script/Packages/";

  return fs.readdirSync(srcpath).filter(function(file) {
    return fs.statSync(path.join(srcpath, file)).isDirectory() && fs.existsSync(srcpath + "/" + file + "/Package.json");
  });

}

// return an object with package name keys and module name lists as values
function getScriptModules(platform) {

  modules = {};

  var packages = getScriptPackages();

  for (var i in packages) {

    var pkg = JSON.parse(fs.readFileSync(atomicRoot + "Script/Packages/" + packages[i] + "/Package.json"));

    if (pkg.platforms && pkg.platforms.indexOf(platform) == -1)
      continue;

    for (var j in pkg.modules) {

      var moduleName = pkg.modules[j];

      if (pkg.moduleExclude && pkg.moduleExclude[platform])
      if (pkg.moduleExclude[platform].indexOf(moduleName) != -1)
        continue;

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

function getGenScriptRootDir(platform) {

  return atomicRoot + "Artifacts/Build/Source/Generated/" + platform + "/";

}

// Get a list of script source filenames for a given platform
function getGenScriptFilenames(platform) {

  var filenames = [];

  var scriptGenRoot = getGenScriptRootDir(platform);

  var modules = getScriptModules(platform);

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

function createGenScriptFiles(platform) {

  var scriptFiles = getGenScriptFilenames(platform);

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
exports.cleanCreateDir = cleanCreateDir;
exports.testCreateDir = testCreateDir;
exports.testRemoveDir = testRemoveDir;
exports.getScriptPackages = getScriptPackages;
exports.getScriptModules = getScriptModules;
exports.getGenScriptFilenames = getGenScriptFilenames;
exports.createGenScriptFiles = createGenScriptFiles;
exports.getGenScriptRootDir = getGenScriptRootDir;
