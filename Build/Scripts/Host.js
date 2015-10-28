var os = require('os');

if (os.platform() == "darwin") {
  module.exports = require("./HostMac");
  require("./BuildMac");
  require("./BuildIOS");
  require("./BuildWeb");
  require("./BuildAndroid");
}
