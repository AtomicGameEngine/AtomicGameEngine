var os = require('os');
var path = require('path');

// get the root folder
var atomicRoot = path.resolve(__dirname, "../..") + "/";

// patch in our local node_modules
process.env.NODE_PATH = atomicRoot + "Build/node_modules/";
require('module').Module._initPaths();
var fs = require('fs-extra');

var artifactsRoot = atomicRoot + "Artifacts/";
var jenkinsBuild = process.env.ATOMIC_JENKINS_BUILD == 1;
var buildSHA = process.env.ATOMIC_BUILD_SHA;

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

function getGenScriptRootDir(platform) {

  return atomicRoot + "Artifacts/Build/Source/Generated/" + platform + "/";

}


function cloneRepo(repoUrl, destPath) {

}

exports.jenkinsBuild = jenkinsBuild;
exports.buildSHA = buildSHA;
exports.atomicRoot = atomicRoot;
exports.artifactsRoot = artifactsRoot;
exports.testRemoveDir = testRemoveDir;
exports.testCreateDir = testCreateDir;
exports.cloneRepo = cloneRepo;
exports.cleanCreateDir = cleanCreateDir;
exports.getGenScriptRootDir = getGenScriptRootDir;
