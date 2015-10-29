var os = require('os');

if (os.platform() == "win32") {
  module.exports = require("./HostWindows");
}
