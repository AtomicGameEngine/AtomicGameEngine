var UI = Atomic.UI;
var UIWidget = Atomic.UIWidget;
var UIListView = Atomic.UIListView;
var fileSystem = Atomic.getFileSystem();

var mainframe = require("./mainframe").mainframe;

var projectframe = exports.projectframe = new UIWidget();

projectframe.gravity = UI.GRAVITY_TOP_BOTTOM;

// load the UI
projectframe.load("AtomicEditor/editor/ui/projectframe.tb.txt");

// snap the project frame into place
var projectviewcontainer = mainframe.getWidget("projectviewcontainer");
projectviewcontainer.addChild(projectframe);

var foldercontainer = projectframe.getWidget("foldercontainer");

var folderList = new UIListView();

folderList.id = "folderList";

foldercontainer.addChild(folderList);

//folderList.addItem("Resources", "Folder.icon", "myid")

var folders = fileSystem.scanDir("/", "", Atomic.SCAN_DIRS, false);

print(folders);
