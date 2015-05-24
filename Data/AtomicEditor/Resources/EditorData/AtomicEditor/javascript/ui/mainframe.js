var graphics = Atomic.getGraphics();

var UI = Atomic.UI;
var UIWidget = Atomic.UIWidget;
var UIMenuWindow = Atomic.UIMenuWindow;

var editorStrings = require("./editorStrings");
var utils = require("./utils");

var view = new Atomic.UIView();

var mainframe = new UIWidget();

mainframe.setSize(graphics.width, graphics.height);

mainframe.load("AtomicEditor/editor/ui/mainframe.tb.txt");

// Subscribe to graphics subsystems screen mode switching
mainframe.subscribeToEvent(graphics, "ScreenMode", function(data) {

  mainframe.setSize(data.width, data.height);

});

mainframe.subscribeToEvent(mainframe, "WidgetEvent", function(data) {

  if (data.type == UI.EVENT_TYPE_CLICK) {

    var target = data.target;

    if (target.id == "menu atomic editor") {

      print ("CLICK! ", target.id);

      var menu = new UIMenuWindow(target, "atomic editor popup");
      menu.show(mainframe.menuAtomicEditorSource);
      return true; // handled

    } else if (target.id == "menu edit") {

      print ("CLICK! ", target.id);

      var menu = new UIMenuWindow(target, "edit popup");
      menu.show(mainframe.menuEditSource);
      return true; // handled
    }

  }

});

initializeMenuSources();

function initializeMenuSources() {

  //var src = mainframe.menuAtomicEditorSource = new UIMenuItemSource();

  var aboutItems = {
    "About Atomic Editor": "about atomic editor",
    "-" : null,
    "Manage License" : "manage license",
    "-" : null,
    "Check for Updates" : "check update",
    "-" : null,
    "Quit" : "quit"

  };

  var editItems = {

    "Undo" : ["edit undo", editorStrings.ShortcutUndo],
    "Redo" : ["edit redo", editorStrings.ShortcutRedo],
    "-" : null,
    "Cut" : ["edit cut", editorStrings.ShortcutCut],
    "Copy" : ["edit copy", editorStrings.ShortcutCopy],
    "Paste" : ["edit paste", editorStrings.ShortcutPaste],
    "Select All" : ["edit select all", editorStrings.ShortcutSelectAll],
    "-" : null,
    "Find" : ["edit find", editorStrings.ShortcutFind],
    "Find Next" : ["edit find next", editorStrings.ShortcutFindNext],
    "Find Prev" : ["edit find prev", editorStrings.ShortcutFindPrev],
    "-" : null,
    "Format Code" : ["edit format code", editorStrings.ShortcutBeautify],
    "-" : null,
    "Play" : ["edit play", editorStrings.ShortcutPlay]
  };

  mainframe.menuAtomicEditorSource = utils.createMenuItemSource(aboutItems);
  mainframe.menuEditSource = utils.createMenuItemSource(editItems);




  /*

  menuFileSource.AddItem(new MenubarItem("New Project", TBIDC("new project")));
  menuFileSource.AddItem(new MenubarItem("Open Project", TBIDC("open project")));
  menuFileSource.AddItem(new MenubarItem("Save Project", TBIDC("save project")));
  menuFileSource.AddItem(new MenubarItem("-"));
  menuFileSource.AddItem(new MenubarItem("Close Project", TBIDC("close project")));
  menuFileSource.AddItem(new MenubarItem("-"));
  menuFileSource.AddItem(new MenubarItem("Save File", TBIDC("save file"), EDITOR_STRING(ShortcutSaveFile)));
  menuFileSource.AddItem(new MenubarItem("Close File", TBIDC("close file"), EDITOR_STRING(ShortcutCloseFile)));

  menuBuildSource.AddItem(new MenubarItem("Build", TBIDC("project_build"), EDITOR_STRING(ShortcutBuild)));
  menuBuildSource.AddItem(new MenubarItem("-"));
  menuBuildSource.AddItem(new MenubarItem("Build Settings", TBIDC("project_build_settings"), EDITOR_STRING(ShortcutBuildSettings)));

  menuToolsSource.AddItem(new MenubarItem("Tiled Map Editor", TBIDC("tools tiled")));


  menuHelpSource.AddItem(new MenubarItem("API Documentation", TBIDC("help_api")));
  menuHelpSource.AddItem(new MenubarItem("-"));
  menuHelpSource.AddItem(new MenubarItem("Forums", TBIDC("help_forums")));
  menuHelpSource.AddItem(new MenubarItem("-"));
  menuHelpSource.AddItem(new MenubarItem("Atomic Game Engine on GitHub", TBIDC("help_github")));

  menuDeveloperSource.AddItem(new MenubarItem("Set 1920x1080 Resolution", TBIDC("developer_resolution")));
  */


}


view.addChild(mainframe);
