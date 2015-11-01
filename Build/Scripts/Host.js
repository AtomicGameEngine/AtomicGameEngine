var os = require('os');

if (os.platform() == "win32") {
  module.exports = require("./HostWindows");
  require("./BuildWindows");
  require("./BuildAndroid");
} else if (os.platform() == "darwin") {
  module.exports = require("./HostMac");
}
