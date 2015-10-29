var os = require('os');

if (os.platform() == "darwin") {
  module.exports = require("./HostMac");
  require("./BuildMac");
  require("./BuildIOS");
  require("./BuildWeb");
  require("./BuildAndroid");
} else if (os.platform() == "win32") {
  module.exports = require("./HostWindows");
  require("./BuildWindows");
}
