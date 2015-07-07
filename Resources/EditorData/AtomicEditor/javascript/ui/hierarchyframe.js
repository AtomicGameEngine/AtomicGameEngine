var UI = Atomic.UI;
var UIWidget = Atomic.UIWidget;

var mainframe = require("./mainframe").mainframe;

var hierarchyframe = exports.hierarchyframe = new UIWidget();

hierarchyframe.gravity = UI.GRAVITY_TOP_BOTTOM;

// load the UI
hierarchyframe.load("AtomicEditor/editor/ui/hierarchyframe.tb.txt");

// snap the frame into place
var hierarchycontainer = mainframe.getWidget("hierarchycontainer");
hierarchycontainer.addChild(hierarchyframe);
