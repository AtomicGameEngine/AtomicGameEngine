
var editorStrings = require("./editorStrings");

var UIMenuItemSource = Atomic.UIMenuItemSource;
var UIMenuItem = Atomic.UIMenuItem;

exports.createMenuItemSource = function(items) {

  var src = new UIMenuItemSource();

  for (var key in items) {
    if (items.hasOwnProperty(key)) {

      var value = items[key];

      if (typeof value === 'string') {

        src.addItem(new UIMenuItem(key, value));

      } else if (value == null) {

        src.addItem(new UIMenuItem(key));

      } else if (typeof value === 'object' && value.length == 2) {

        src.addItem(new UIMenuItem(key, value[0], editorStrings.str(value[1])));

      }


    }

  }

  return src;

}
