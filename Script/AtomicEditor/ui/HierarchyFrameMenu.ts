
import strings = require("./EditorStrings");
import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");
import MenuItemSources = require("./menus/MenuItemSources");

class HierarchyFrameMenus extends Atomic.ScriptObject {

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("hierarchy create items", createItems);

        this.subscribeToEvent(EditorEvents.ContentFolderChanged, (ev: EditorEvents.ContentFolderChangedEvent) => {
            this.contentFolder = ev.path;
        })

    }

    handlePopupMenu(target: Atomic.UIWidget, refid: string, node: Atomic.Node): boolean {

        if (!target || !refid) return;

        if (target.id == "create popup") {

            var child:Atomic.Node;

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

              this.sendEvent(EditorEvents.ActiveNodeChange, { node: child });

            }

            return true;

        }

        return false;

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
