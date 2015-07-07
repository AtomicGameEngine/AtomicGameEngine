


exports.RevealInFinder = 0;
exports.ShortcutUndo = 1;
exports.ShortcutRedo = 2;
exports.ShortcutCut = 3;
exports.ShortcutCopy = 4;
exports.ShortcutPaste = 5;
exports.ShortcutSelectAll = 6;
exports.ShortcutFind = 7;
exports.ShortcutFindNext = 8;
exports.ShortcutFindPrev = 9;
exports.ShortcutBeautify = 10;
exports.ShortcutCloseFile = 11;
exports.ShortcutSaveFile = 12;
exports.ShortcutPlay = 13;
exports.ShortcutBuild = 14;
exports.ShortcutBuildSettings = 15;

exports.str = function(value) {

  var string = strings[value];

  if (typeof string === 'undefined') {
      return "";
  }

  return string;
}

var strings = {};

var shortcutKey = "⌘";

strings[exports.RevealInFinder] = "Reveal in Finder";

// Mac
strings[exports.ShortcutRedo] ="⇧⌘Z";
strings[exports.ShortcutFindNext] = "⌘G";
strings[exports.ShortcutFindPrev] = "⇧⌘G";
strings[exports.ShortcutBuildSettings] = "⇧⌘B";


// General
strings[exports.ShortcutUndo] = shortcutKey + "Z";

strings[exports.ShortcutCut] = shortcutKey + "X";
strings[exports.ShortcutCopy] = shortcutKey + "C";
strings[exports.ShortcutPaste] = shortcutKey + "V";
strings[exports.ShortcutSelectAll] = shortcutKey + "A";
strings[exports.ShortcutFind] = shortcutKey + "F";

strings[exports.ShortcutBeautify] = shortcutKey + "I";

strings[exports.ShortcutSaveFile] = shortcutKey + "S";
strings[exports.ShortcutCloseFile] = shortcutKey + "W";

strings[exports.ShortcutPlay] = shortcutKey + "P";

strings[exports.ShortcutBuild] = shortcutKey +"B";
