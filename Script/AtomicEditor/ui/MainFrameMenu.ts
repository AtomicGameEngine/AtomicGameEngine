
import strings = require("./EditorStrings");
import EditorEvents = require("../editor/EditorEvents");

var UIMenuItemSource = Atomic.UIMenuItemSource;
var UIMenuItem = Atomic.UIMenuItem;

var StringID = strings.StringID;

class MainFrameMenu extends Atomic.ScriptObject {

  constructor() {

    super();

    this.srcLookup["menu atomic editor"] = this.createMenuItemSource(editorItems);
    this.srcLookup["menu edit"] = this.createMenuItemSource(editItems);
    this.srcLookup["menu file"] = this.createMenuItemSource(fileItems);

  }

  handlePopupMenu(target: Atomic.UIWidget, refid: string): boolean {

      if (target.id == "menu atomic editor popup") {

          if (refid == "quit") {

            this.sendEvent(EditorEvents.Quit);
            return true;

          }

      } else if (target.id == "menu edit popup") {

          if (refid == "edit play") {

              new ToolCore.PlayCmd().run();
              return true;

          }

          return false;

      } else if (target.id == "menu file popup") {

          if (refid == "file save file") {

             //TODO: this is horrible
              //if (this.resourceframe.currentResourceEditor)
              //    this.resourceframe.currentResourceEditor.save();

              return true;

          }

          return false;
      }

  }

  getMenuItemSource(id: string): Atomic.UIMenuItemSource {

      return this.srcLookup[id];

  }

  createMenuItemSource(items: any): Atomic.UIMenuItemSource {

      var src = new UIMenuItemSource();

      for (var key in items) {

          if (items.hasOwnProperty(key)) {

              var value = items[key];

              if (typeof value === 'string') {

                  src.addItem(new UIMenuItem(key, value));

              } else if (value == null) {

                  src.addItem(new UIMenuItem(key));

              }
              else if (typeof value === 'object' && value.length == 1) {

                  src.addItem(new UIMenuItem(key, value[0]));

              } else if (typeof value === 'object' && value.length == 2) {

                  src.addItem(new UIMenuItem(key, value[0], strings.EditorString.GetString(value[1])));

              }

          }

      }

      return src;

  }

  srcLookup = {};

}

export = MainFrameMenu;

// initialization

var editorItems = {
    "About Atomic Editor": "about atomic editor",
    "-1": null,
    "Manage License": "manage license",
    "-2": null,
    "Check for Updates": "check update",
    "-3": null,
    "Quit": "quit"
};

var editItems = {

    "Undo": ["edit undo", StringID.ShortcutUndo],
    "Redo": ["edit redo", StringID.ShortcutRedo],
    "-1": null,
    "Cut": ["edit cut", StringID.ShortcutCut],
    "Copy": ["edit copy", StringID.ShortcutCopy],
    "Paste": ["edit paste", StringID.ShortcutPaste],
    "Select All": ["edit select all", StringID.ShortcutSelectAll],
    "-2": null,
    "Find": ["edit find", StringID.ShortcutFind],
    "Find Next": ["edit find next", StringID.ShortcutFindNext],
    "Find Prev": ["edit find prev", StringID.ShortcutFindPrev],
    "-3": null,
    "Format Code": ["edit format code", StringID.ShortcutBeautify],
    "-4": null,
    "Play": ["edit play", StringID.ShortcutPlay]

};

var fileItems = {

  "New Project": ["file new project"],
  "Open Project": ["file open project"],
  "Save Project": ["file save project"],
  "-1": null,
  "Close Project": ["file close project"],
  "-2": null,
  "Save File": ["file save file"],
  "Close File": ["file close file"]
}
