//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import strings = require("ui/EditorStrings");
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");

var UIMenuItemSource = Atomic.UIMenuItemSource;
var UIMenuItem = Atomic.UIMenuItem;
var StringID = strings.StringID;

var srcLookup = {};

export function getMenuItemSource(id: string): Atomic.UIMenuItemSource {

    return srcLookup[id];

}

function createMenuItemSourceRecursive(items: any): Atomic.UIMenuItemSource {

    var src = new UIMenuItemSource();

    for (var key in items) {

        if (items.hasOwnProperty(key)) {

            var value = items[key];

            if (typeof value === 'string') {

                src.addItem(new UIMenuItem(key, value));

            } else if (value == null) {

                // add a separator
                src.addItem(new UIMenuItem(key));

            } else if (Object.prototype.toString.call(value) === '[object Array]') {

                if (value.length == 1)
                    src.addItem(new UIMenuItem(key, value[0]));
                else if (value.length == 2)
                    src.addItem(new UIMenuItem(key, value[0], strings.EditorString.GetString(value[1])));
                else if (value.length == 3) {

                    var str = "";
                    if (value[1])
                        str = strings.EditorString.GetString(value[1]);

                    var skinBg = "";
                    if (value[2])
                        skinBg = value[2];

                    src.addItem(new UIMenuItem(key, value[0], str, skinBg));

                }

            }
            else if (typeof value === 'object') {

                var subsrc = createMenuItemSourceRecursive(value);

                var item = new Atomic.UIMenuItem(key);
                item.subSource = subsrc;
                src.addItem(item);

            }


        }

    }

    return src;

}

export function createMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource {

    srcLookup[id] = createMenuItemSourceRecursive(items);

    return srcLookup[id];

}
