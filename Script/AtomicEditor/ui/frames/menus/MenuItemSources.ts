//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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

            if (typeof value === "string") {

                src.addItem(new UIMenuItem(key, value));

            } else if (value == null) {

                // add a separator
                src.addItem(new UIMenuItem(key));

            } else if (Object.prototype.toString.call(value) === "[object Array]") {

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
            else if (typeof value === "object") {
                createSubMenuItemSource(src, key, value);

            }


        }

    }

    return src;

}

export function createSubMenuItemSource(src: Atomic.UIMenuItemSource, id: string, items: any): Atomic.UIMenuItemSource {
    var subsrc = createMenuItemSourceRecursive(items);

    var item = new Atomic.UIMenuItem(id);
    item.subSource = subsrc;
    src.addItem(item);

    return subsrc;
}

export function createMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource {

    srcLookup[id] = createMenuItemSourceRecursive(items);

    return srcLookup[id];

}

export function deleteMenuItemSource(id: string) {
    if (srcLookup[id]) {
        delete srcLookup[id];
    }
}
