var os = require('os');
var path = require('path');

// get the root folder
var atomicRoot = path.resolve(__dirname, "../..") + "/";

// patch in our local node_modules
process.env.NODE_PATH = atomicRoot + "Build/node_modules/";
require('module').Module._initPaths();
var fs = require('fs-extra');

function getScriptPackages() {

    var srcpath = atomicRoot + "Script/Packages/";

    return fs.readdirSync(srcpath).filter(function(file) {
      return fs.statSync(path.join(srcpath, file)).isDirectory();
    });
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
