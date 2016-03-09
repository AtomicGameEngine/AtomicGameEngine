var os = require('os');

if (os.platform() == "win32") {
  module.exports = require("./HostWindows");
  require("./BuildWindows");
} else if (os.platform() == "darwin") {
  module.exports = require("./HostMac");
  require("./BuildMac");
} else if (os.platform() == "linux") {
  module.exports = require("./HostLinux");
  require("./BuildLinux");
}

require("./BuildCommon");
require("./BuildAndroid");
require("./BuildWeb");
