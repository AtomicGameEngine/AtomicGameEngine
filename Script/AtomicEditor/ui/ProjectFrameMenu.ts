
import strings = require("./EditorStrings");
import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");
import MenuItemSources = require("./menus/MenuItemSources");

class ProjectFrameMenus extends Atomic.ScriptObject {

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("asset context folder", assetFolderContextItems);
        MenuItemSources.createMenuItemSource("asset context general", assetGeneralContextItems);
        MenuItemSources.createMenuItemSource("project create items", createItems);

        this.subscribeToEvent(EditorEvents.ContentFolderChanged, (ev: EditorEvents.ContentFolderChangedEvent) => {
            this.contentFolder = ev.path;
        })

    }

    handleAssetContextMenu(target: Atomic.UIWidget, refid: string) {

        if (target.id == "asset context menu") {

            var asset = <ToolCore.Asset> target['asset'];

            if (refid == "delete_asset") {
                EditorUI.getModelOps().showResourceDelete(asset);
                return true;
            }

            if (refid == "create_folder") {
                EditorUI.getModelOps().showCreateFolder(asset.path);
                return true;
            }

            if (refid == "create_component") {
                EditorUI.getModelOps().showCreateComponent(asset.path);
                return true;
            }

        }

    }

    createFolderContextMenu(parent: Atomic.UIWidget, id: string, folder: ToolCore.Asset, x: number, y: number) {


    }

    createAssetContextMenu(parent: Atomic.UIWidget, asset: ToolCore.Asset, x: number, y: number) {

        var menu = new Atomic.UIMenuWindow(parent, "asset context menu");
        menu['asset'] = asset;

        var srcName: string;

        if (asset.isFolder()) {
            srcName = "asset context folder";
        } else {
            srcName = "asset context general";
        }

        var src = MenuItemSources.getMenuItemSource(srcName);
        menu.show(src, x, y);

    }

    handlePopupMenu(target: Atomic.UIWidget, refid: string): boolean {

        if (!target || !refid) return;

        if (this.handleAssetContextMenu(target, refid)) {

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

var assetGeneralContextItems = {
    "Reveal in Finder": ["reveal_folder", undefined, ""],
    "-1": null,
    "Delete": ["delete_asset", undefined, ""]
};

var assetFolderContextItems = {
    "Create Folder": ["create_folder", undefined, "Folder.icon"],
    "Create Component": ["create_component", undefined, "ComponentBitmap"],
    "-1": null,
    "Reveal in Finder": ["reveal_folder", undefined, ""],
    "-2": null,
    "Delete": ["delete_asset", undefined, "FolderDeleteBitmap"]
};

var createItems = {
    "Folder": ["create_folder", undefined, "Folder.icon"],
    "-1": null,
    "Script": ["create_script", undefined, "JavascriptBitmap"]
};
