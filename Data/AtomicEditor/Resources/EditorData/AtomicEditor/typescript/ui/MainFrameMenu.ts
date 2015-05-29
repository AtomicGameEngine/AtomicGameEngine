
import strings = require("./EditorStrings");

var UIMenuItemSource = Atomic.UIMenuItemSource;
var UIMenuItem = Atomic.UIMenuItem;

var StringID = strings.StringID;

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

var srcLookup = {};

srcLookup["menu atomic editor"] = createMenuItemSource(editorItems);
srcLookup["menu edit"] = createMenuItemSource(editItems);

export function getMenuItemSource(id: string): Atomic.UIMenuItemSource {
  return srcLookup[id];
}

function createMenuItemSource(items: any): Atomic.UIMenuItemSource {

  var src = new UIMenuItemSource();

  for (var key in items) {

    if (items.hasOwnProperty(key)) {

      var value = items[key];

      if (typeof value === 'string') {

        src.addItem(new UIMenuItem(key, value));

      } else if (value == null) {

        src.addItem(new UIMenuItem(key));

      } else if (typeof value === 'object' && value.length == 2) {

        src.addItem(new UIMenuItem(key, value[0], strings.EditorString.GetString(value[1])));

      }

    }

  }

  return src;

}

