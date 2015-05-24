
// create the view first so it exists before requiring additional modules
var view = new Atomic.UIView();
exports.view = view;

require("./mainframe");
require("./projectframe");
require("./hierarchyframe");
