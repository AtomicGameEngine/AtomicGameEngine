//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import strings = require("ui/EditorStrings");
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import MenuItemSources = require("./MenuItemSources");

class HierarchyFrameMenus extends Atomic.ScriptObject {

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("hierarchy create items", createItems);
        MenuItemSources.createMenuItemSource("node context general", nodeGeneralContextItems);

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

            } else if (refid == "duplicate_node") {

                if (node instanceof Atomic.Scene)
                    return;

                var newnode = node.clone();
                node.scene.sendEvent("SceneEditNodeCreated", { node: newnode });
            }

            return true;
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

    contentFolder: string;

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
