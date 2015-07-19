
import strings = require("./EditorStrings");
import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");
import MenuItemSources = require("./menus/MenuItemSources");

class ProjectFrameMenus extends Atomic.ScriptObject {

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("project context folder", folderContextItems);
        MenuItemSources.createMenuItemSource("project create items", createItems);

        this.subscribeToEvent(EditorEvents.ContentFolderChanged, (ev: EditorEvents.ContentFolderChangedEvent) => {
            this.contentFolder = ev.path;
        })

    }

    handleFolderContextMenu(target: Atomic.UIWidget, refid: string) {

        if (target.id == "folder context menu") {

            var folder = <ToolCore.Asset> target['folder'];

            if (refid == "delete_folder") {

                EditorUI.getModelOps().showResourceDelete(folder);

                return true;
            }

            if (refid == "create_folder") {

                EditorUI.getModelOps().showCreateFolder(folder.path);

                return true;

            }

        }

    }

    createFolderContextMenu(parent: Atomic.UIWidget, id: string, folder: ToolCore.Asset, x: number, y: number) {

        var menu = new Atomic.UIMenuWindow(parent, id);
        menu['folder'] = folder;
        var src = MenuItemSources.getMenuItemSource("project context folder");
        menu.show(src, x, y);

    }

    handlePopupMenu(target: Atomic.UIWidget, refid: string): boolean {

        if (!target || !refid) return;

        if (this.handleFolderContextMenu(target, refid)) {

            return true;

        }

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

    contentFolder: string;

}

export = ProjectFrameMenus;

// initialization
var StringID = strings.StringID;

var folderContextItems = {
    "Create Folder": ["create_folder", undefined, "Folder.icon"],
    "Reveal in Finder": ["reveal_folder", undefined, ""],
    "-1": null,
    "Delete": ["delete_folder", undefined, "FolderDeleteBitmap"]
};

var createItems = {
    "Folder": ["create_folder", undefined, "Folder.icon"],
    "-1": null,
    "Script": ["create_script", undefined, "JavascriptBitmap"]
};
