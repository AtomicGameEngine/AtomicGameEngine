

import scriptwidget = require("./ScriptWidget");

export class ProjectFrame extends scriptwidget.ScriptWidget {

  folderList:Atomic.UIListView;

	constructor(parent: Atomic.UIWidget) {

		super();

		this.load("AtomicEditor/editor/ui/projectframe.tb.txt");

    this.gravity = Atomic.UI.GRAVITY_TOP_BOTTOM;

    var projectviewcontainer = parent.getWidget("projectviewcontainer");

    projectviewcontainer.addChild(this);

    var foldercontainer = this.getWidget("foldercontainer");

    var folderList = this.folderList = new Atomic.UIListView();

    folderList.id = "folderList";

    foldercontainer.addChild(folderList);

    folderList.addItem("Resources", "Folder.icon", "myid")

    var fileSystem = Atomic.getFileSystem();

    var folders = fileSystem.scanDir("/", "", Atomic.SCAN_DIRS, false);

    print(folders);



	}

	onEventClick(target: Atomic.UIWidget, refid: string): boolean {

      return false;

	}


}



/*
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
*/
