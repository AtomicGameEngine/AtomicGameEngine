var graphics = Atomic.getGraphics();
var UI = Atomic.UI;
var UIWidget = Atomic.UIWidget;
var UIMenuWindow = Atomic.UIMenuWindow;
var editorStrings = require("./editorStrings");
var utils = require("./utils");
var ui = require("./ui");

// Create the main frame
var mainframe = exports.mainframe = new UIWidget();

ui.view.addChild(mainframe);

// Set it to be the size of the entire window
mainframe.setSize(graphics.width, graphics.height);

// load the UI
mainframe.load("AtomicEditor/editor/ui/mainframe.tb.txt");

// Subscribe to graphics subsystems screen mode switching, so we can adjust the widget size
mainframe.subscribeToEvent(graphics, "ScreenMode", function(data) {

  mainframe.setSize(data.width, data.height);

});

mainframe.handleMenuAtomicEditor = function(data) {

  var target = data.target;

  if (target && target.id == "menu atomic editor popup") {

    if (data.refid == "quit") {

      // todo, send a request for file saves, etc
      Atomic.getEngine().exit();

      return true;

    }

  }

  if (target && target.id == "menu edit popup") {

    if (data.refid == "edit play") {

      new ToolCore.PlayCmd().run();

      return true;

    }

  }


}

mainframe.handleMenuBarEvent = function(data) {

  if (data.type == UI.EVENT_TYPE_CLICK) {

    if (mainframe.handleMenuAtomicEditor(data)) return true;

    var target = data.target;

    var src = srcLookup[target.id];

    if (src) {

      var menu = new UIMenuWindow(target, target.id + " popup");
      menu.show(src);
      return true;

    }

  }

  return false;

}

mainframe.subscribeToEvent(mainframe, "WidgetEvent", function(data) {

  if (mainframe.handleMenuBarEvent(data)) return true;

  return false;

});

var srcLookup = {};

var editorItems = {
  "About Atomic Editor": "about atomic editor",
  "-1" : null,
  "Manage License" : "manage license",
  "-2" : null,
  "Check for Updates" : "check update",
  "-3" : null,
  "Quit" : "quit"
};

var editItems = {

  "Undo" : ["edit undo", editorStrings.ShortcutUndo],
  "Redo" : ["edit redo", editorStrings.ShortcutRedo],
  "-1" : null,
  "Cut" : ["edit cut", editorStrings.ShortcutCut],
  "Copy" : ["edit copy", editorStrings.ShortcutCopy],
  "Paste" : ["edit paste", editorStrings.ShortcutPaste],
  "Select All" : ["edit select all", editorStrings.ShortcutSelectAll],
  "-2" : null,
  "Find" : ["edit find", editorStrings.ShortcutFind],
  "Find Next" : ["edit find next", editorStrings.ShortcutFindNext],
  "Find Prev" : ["edit find prev", editorStrings.ShortcutFindPrev],
  "-3" : null,
  "Format Code" : ["edit format code", editorStrings.ShortcutBeautify],
  "-4" : null,
  "Play" : ["edit play", editorStrings.ShortcutPlay]
};

var fileItems = {
  "New Project" : "new project",
  "Open Project" : "open project",
  "Save Project" : "save project",
  "-1" : null,
  "Close Project" : "close project",
  "-2" : null,
  "Save File" : ["save file", editorStrings.ShortcutSaveFile],
  "Close File" : ["close file", editorStrings.ShortcutCloseFile],

};

var buildItems = {
  "Build" : ["build project", editorStrings.ShortcutBuild],
  "-1" : null,
  "Build Settings" : ["build project settings", editorStrings.ShortcutBuildSettings],
};

var toolsItems = {
  "Tiled Map Editor" : "tools tiles"
};

var helpItems = {
  "API Documentation" : "help api",
  "-1" : null,
  "Forums" : "help forums",
  "-2" : null,
  "Atomic Game Engine on GitHub" : "help github"
};

var developerItems = {
  "Set 1920x1080 Resolution" : "developer resolution"
};

srcLookup["menu atomic editor"] = utils.createMenuItemSource(editorItems);
srcLookup["menu file"] = utils.createMenuItemSource(fileItems);
srcLookup["menu edit"] = utils.createMenuItemSource(editItems);
srcLookup["menu build"] = utils.createMenuItemSource(buildItems);
srcLookup["menu tools"] = utils.createMenuItemSource(toolsItems);
srcLookup["menu help"] = utils.createMenuItemSource(helpItems);
srcLookup["menu developer"] = utils.createMenuItemSource(developerItems);
