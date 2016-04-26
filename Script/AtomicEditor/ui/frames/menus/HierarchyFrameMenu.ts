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
import MenuItemSources = require("./MenuItemSources");
import ServiceLocator from "../../../hostExtensions/ServiceLocator";

class HierarchyFrameMenus extends Atomic.ScriptObject {

    contentFolder: string;

    private contextMenuItemSource: Atomic.UIMenuItemSource = null;

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("hierarchy create items", createItems);
        this.contextMenuItemSource = MenuItemSources.createMenuItemSource("node context general", nodeGeneralContextItems);

        this.subscribeToEvent(EditorEvents.ContentFolderChanged, (ev: EditorEvents.ContentFolderChangedEvent) => {
            this.contentFolder = ev.path;
        });

    }

    handlePopupMenu(target: Atomic.UIWidget, refid: string, node: Atomic.Node): boolean {

        if (!target || !refid) return false;

        if (target.id == "create popup") {

            var child: Atomic.Node;

            if (refid == "create_node") {

                if (node) {

                    child = node.createChild("Node");
                }

            }

            else if (refid == "create_light") {

                if (node) {

                    child = node.createChild("Light");
                    child.createComponent("Light");

                }

            }

            if (child) {
                child.scene.sendEvent("SceneEditNodeCreated", { node: child });
            }

            return true;

        }

        return false;

    }

    handleNodeContextMenu(target: Atomic.UIWidget, refid: string): boolean {

        if (target.id == "node context menu") {

            var node = <Atomic.Node>target["node"];

            if (!node) {
                return false;
            }

            if (refid == "delete_node") {

                if (node instanceof Atomic.Scene)
                    return;

                var scene = node.scene;
                scene.sendEvent("SceneEditAddRemoveNodes", { end: false });
                scene.sendEvent("SceneEditNodeRemoved", { node: node, parent: node.parent, scene: scene });
                node.remove();
                scene.sendEvent("SceneEditAddRemoveNodes", { end: true });

                return true;

            } else if (refid == "duplicate_node") {

                if (node instanceof Atomic.Scene)
                    return;

                var newnode = node.clone();
                node.scene.sendEvent("SceneEditNodeCreated", { node: newnode });

                return true;
            }

            // Let plugins handle context
            return ServiceLocator.uiServices.hierarchyContextItemClicked(node, refid);
        }

        return false;

    }


    createNodeContextMenu(parent: Atomic.UIWidget, node: Atomic.Node, x: number, y: number) {


        var menu = new Atomic.UIMenuWindow(parent, "node context menu");

        menu["node"] = node;

        var srcName: string = "node context general";

        var src = MenuItemSources.getMenuItemSource(srcName);
        menu.show(src, x, y);


    }

    createPluginItemSource(id: string, items: any): Atomic.UIMenuItemSource {
        return MenuItemSources.createSubMenuItemSource(this.contextMenuItemSource , id, items);
    }

    removePluginItemSource(id: string) {
        this.contextMenuItemSource.removeItemWithStr(id);
    }

}

export = HierarchyFrameMenus;

// initialization
var StringID = strings.StringID;

var createItems = {
    Node: ["create_node", undefined, "Folder.icon"],
    "-1": null,
    "3D": {
        Light: ["create_light", undefined, "JavascriptBitmap"]
    }
};

var nodeGeneralContextItems = {
    "Duplicate": ["duplicate_node", undefined, ""],
    "-1": null,
    "Delete": ["delete_node", undefined, ""]
};
