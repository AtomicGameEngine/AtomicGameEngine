

var mode = process.env.ATOMIC_GAME_ENGINE_ENV || 'production';

var atomictool = require ("./atomictool.js");
var atomiceditor = require ("./atomiceditor.js");

module.exports = {

  "mode" : mode,
  "newProject" : atomictool.newProject,
  "addPlatform" : atomictool.addPlatform,
  "build" : atomictool.build,
  "run" : atomictool.run,
  "activate" : atomictool.activate,
  "deactivate" : atomictool.deactivate,
  "atomiceditor" : atomiceditor

}
