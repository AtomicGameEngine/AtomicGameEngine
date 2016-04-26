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

class ProjectFrameMenus extends Atomic.ScriptObject {

    contentFolder: string;

    private contextMenuItemSource: Atomic.UIMenuItemSource = null;

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("asset context folder", assetFolderContextItems);
        this.contextMenuItemSource = MenuItemSources.createMenuItemSource("asset context general", assetGeneralContextItems);
        MenuItemSources.createMenuItemSource("project create items", createItems);

        this.subscribeToEvent(EditorEvents.ContentFolderChanged, (ev: EditorEvents.ContentFolderChangedEvent) => {
            this.contentFolder = ev.path;
        });

    }

    handleAssetContextMenu(target: Atomic.UIWidget, refid: string):boolean {

        if (target.id == "asset context menu" || target.id == "create popup") {

            var path;
            var asset = <ToolCore.Asset> target["asset"];

            if (asset) {
                path = asset.path;
            } else {
                path = this.contentFolder;
            }

            if (refid == "rename_asset") {
                EditorUI.getModelOps().showRenameAsset(asset);
                return true;
            }

            if (refid == "delete_asset") {
                EditorUI.getModelOps().showResourceDelete(asset);
                return true;
            }

            if (refid == "create_folder") {
                EditorUI.getModelOps().showCreateFolder(path);
                return true;
            }

            if (refid == "create_component") {
                EditorUI.getModelOps().showCreateComponent(path);
                return true;
            }

            if (refid == "create_script") {
                EditorUI.getModelOps().showCreateScript(path);
                return true;
            }

            if (refid == "create_scene") {
                EditorUI.getModelOps().showCreateScene(path);
                return true;
            }

            if (refid == "create_material") {
                EditorUI.getModelOps().showCreateMaterial(path);
                return true;
            }

            if (refid == "reveal_folder") {
                var utils = new Editor.FileUtils();
                utils.revealInFinder(path);
                return true;
            }

            if (refid == "force_reimport") {
                asset.setDirty(true);
                ToolCore.assetDatabase.scan();
                return true;
            }

            if (refid == "force_reimport_folder") {
                ToolCore.assetDatabase.reimportAllAssetsInDirectory(path);
                return true;
            }

            // Let plugins handle context
            return ServiceLocator.uiServices.projectContextItemClicked(asset, refid);
        }

        return false;

    }

    createFolderContextMenu(parent: Atomic.UIWidget, id: string, folder: ToolCore.Asset, x: number, y: number) {


    }

    createAssetContextMenu(parent: Atomic.UIWidget, asset: ToolCore.Asset, x: number, y: number) {

        var menu = new Atomic.UIMenuWindow(parent, "asset context menu");
        menu["asset"] = asset;

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

        return false;

    }

    createPluginItemSource(id: string, items: any): Atomic.UIMenuItemSource {
        return MenuItemSources.createSubMenuItemSource(this.contextMenuItemSource , id, items);
    }

    removePluginItemSource(id: string) {
        this.contextMenuItemSource.removeItemWithStr(id);
    }

}

export = ProjectFrameMenus;

// initialization
var StringID = strings.StringID;

//Change the words "Reveal in Finder" based on platform
var showInFs = "Reveal in File Manager";

if (Atomic.platform == "Windows") {
    showInFs = "Reveal in Explorer";
}
else if (Atomic.platform == "MacOSX") {
    showInFs = "Reveal in Finder";
}


var assetGeneralContextItems = {
    "Rename": ["rename_asset", undefined, ""],
    "Force Reimport": ["force_reimport", undefined, ""],
    [showInFs]: ["reveal_folder", undefined, ""],
    "-1": null,
    "Delete": ["delete_asset", undefined, ""]
};

var assetFolderContextItems = {
    "Create Folder": ["create_folder", undefined, "Folder.icon"],
    "Create Component": ["create_component", undefined, "ComponentBitmap"],
    "Create Script": ["create_script", undefined, "ComponentBitmap"],
    "Create Material": ["create_material", undefined, "ComponentBitmap"],
    "Create Scene": ["create_scene", undefined, "ComponentBitmap"],
    "Force Reimport": ["force_reimport_folder", undefined, ""],
    "-1": null,
    [showInFs]: ["reveal_folder", undefined, ""],
    "-2": null,
    "Delete": ["delete_asset", undefined, "FolderDeleteBitmap"]
};

var createItems = {
    "Create Folder": ["create_folder", undefined, "Folder.icon"],
    "Create Component": ["create_component", undefined, "ComponentBitmap"],
    "Create Script": ["create_script", undefined, "ComponentBitmap"],
    "Create Material": ["create_material", undefined, "ComponentBitmap"],
    "Create Scene": ["create_scene", undefined, "ComponentBitmap"],
};
