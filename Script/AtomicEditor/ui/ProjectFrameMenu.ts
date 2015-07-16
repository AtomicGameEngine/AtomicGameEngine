
import strings = require("./EditorStrings");
import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");
import MenuItemSources = require("./menus/MenuItemSources");

class ProjectFrameMenus extends Atomic.ScriptObject {

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("project create items", createItems);

        this.subscribeToEvent(EditorEvents.ContentFolderChanged, (ev:EditorEvents.ContentFolderChangedEvent) => {
          this.contentFolder = ev.path;
        })

    }

    handlePopupMenu(target: Atomic.UIWidget, refid: string): boolean {

        if (!target || !refid) return;

        if (target.id == "create popup") {

            if (refid == "create_folder") {

                EditorUI.getModelOps().showCreateFolder(this.contentFolder);

                return true;

            }

            if (refid == "create_script") {

                return true;

            }

        }

        return false;

    }

    contentFolder:string;

}

export = ProjectFrameMenus;

// initialization
var StringID = strings.StringID;

var createItems = {
    "Folder": ["create_folder", undefined, "Folder.icon"],
    "-1": null,
    "Script": ["create_script", undefined, "JavascriptBitmap"]
};
