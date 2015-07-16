
import strings = require("../EditorStrings");
import EditorEvents = require("../../editor/EditorEvents");
import EditorUI = require("../EditorUI");

var UIMenuItemSource = Atomic.UIMenuItemSource;
var UIMenuItem = Atomic.UIMenuItem;
var StringID = strings.StringID;

var srcLookup = {};

export function getMenuItemSource(id: string): Atomic.UIMenuItemSource {

    return srcLookup[id];

}

export function createMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource {

    var src = srcLookup[id] = new UIMenuItemSource();

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

            } else if (typeof value === 'object' && value.length == 3) {

                var str = "";
                if (value[1])
                    str = strings.EditorString.GetString(value[1]);

                var skinBg = "";
                if (value[2])
                    skinBg = value[2];

                src.addItem(new UIMenuItem(key, value[0], str, skinBg));

            }

        }

    }

    return src;

}
