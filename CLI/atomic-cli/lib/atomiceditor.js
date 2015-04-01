
var spawn = require("child_process").spawn;

try {
  var platform_cli = require('atomic-cli-mac');
}
catch (e) {
}
try {
  var platform_cli = require('atomic-cli-windows');
}
catch (e) {
}

var atomiceditor = function (flags, opts) {
  opts = opts || {};
  opts.detached = true;
  opts.stdio = ["ignore", "ignore", "ignore"];
  var child = spawn(platform_cli.ATOMICEDITOR_APP, flags, opts);
  child.unref();
};

module.exports = atomiceditor;
