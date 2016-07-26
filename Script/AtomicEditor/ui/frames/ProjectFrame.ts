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

import ScriptWidget = require("ui/ScriptWidget");
import Editor = require("editor/Editor");
import EditorEvents = require("editor/EditorEvents");
import ProjectFrameMenu = require("./menus/ProjectFrameMenu");
import MenuItemSources = require("./menus/MenuItemSources");
import SearchBarFiltering = require("resources/SearchBarFiltering");

class ProjectFrame extends ScriptWidget {

    folderList: Atomic.UIListView;
    menu: ProjectFrameMenu;
    currentFolder: ToolCore.Asset;
    resourceFolder: ToolCore.Asset;
    assetGUIDToItemID = {};
    resourcesID: number = -1;
    assetReferencePath: string = null;
    currentReferencedButton: Atomic.UIButton = null;
    containerScrollToHeight: number;
    containerScrollToHeightCounter: number;
    uiSearchBar: SearchBarFiltering.UISearchBar = new SearchBarFiltering.UISearchBar();
    search: boolean = false;
    searchEdit: Atomic.UIEditField;

    constructor(parent: Atomic.UIWidget) {

        super();

        this.menu = new ProjectFrameMenu();

        this.load("AtomicEditor/editor/ui/projectframe.tb.txt");

        this.gravity = Atomic.UI_GRAVITY_TOP_BOTTOM;

        this.searchEdit = <Atomic.UIEditField>this.getWidget("filter");

        var projectviewcontainer = parent.getWidget("projectviewcontainer");

        projectviewcontainer.addChild(this);

        var foldercontainer = this.getWidget("foldercontainer");

        var folderList = this.folderList = new Atomic.UIListView();

        folderList.rootList.id = "folderList_";

        foldercontainer.addChild(folderList);

        // events
        this.subscribeToEvent("ProjectLoaded", (data) => this.handleProjectLoaded(data));
        this.subscribeToEvent(EditorEvents.ProjectUnloadedNotification, (data) => this.handleProjectUnloaded(data));
        this.subscribeToEvent("DragEnded", (data: Atomic.DragEndedEvent) => this.handleDragEnded(data));

        this.subscribeToEvent("ResourceAdded", (ev: ToolCore.ResourceAddedEvent) => this.handleResourceAdded(ev));
        this.subscribeToEvent("ResourceRemoved", (ev: ToolCore.ResourceRemovedEvent) => this.handleResourceRemoved(ev));
        this.subscribeToEvent("AssetRenamed", (ev: ToolCore.AssetRenamedEvent) => this.handleAssetRenamed(ev));
        this.subscribeToEvent(EditorEvents.InspectorProjectReference, (ev: EditorEvents.InspectorProjectReferenceEvent) => { this.handleInspectorProjectReferenceHighlight(ev.path); });

        folderList.subscribeToEvent("UIListViewSelectionChanged", (event: Atomic.UIListViewSelectionChangedEvent) => this.handleFolderListSelectionChangedEvent(event));

        // this.subscribeToEvent(EditorEvents.ResourceFolderCreated, (ev: EditorEvents.ResourceFolderCreatedEvent) => this.handleResourceFolderCreated(ev));

        // this uses FileWatcher which doesn't catch subfolder creation
        this.subscribeToEvent("FileChanged", (data) => {

            // console.log("File CHANGED! ", data.fileName);

        });

        // Activates search while user is typing in search widget
        this.searchEdit.subscribeToEvent(this.searchEdit, "WidgetEvent", (data) => {

            if (!ToolCore.toolSystem.project) return;

            if (data.type == Atomic.UI_EVENT_TYPE_KEY_UP) {
                this.search = true;
                this.refreshContent(this.currentFolder);
            }
        });

    }

    handleAssetRenamed(ev: ToolCore.AssetRenamedEvent) {

        var container: Atomic.UILayout = <Atomic.UILayout>this.getWidget("contentcontainer");

        for (var widget = container.firstChild; widget; widget = widget.next) {

            if (widget.id == ev.asset.guid) {

                if (widget["assetButton"]) {
                    widget["assetButton"].text = ev.asset.name + ev.asset.extension;
                    widget["assetButton"].dragObject = new Atomic.UIDragObject(ev.asset, ev.asset.name);
                }

                break;
            }
        }

    }

    handleResourceRemoved(ev: ToolCore.ResourceRemovedEvent) {

        var folderList = this.folderList;
        folderList.deleteItemByID(ev.guid);

        var container: Atomic.UILayout = <Atomic.UILayout>this.getWidget("contentcontainer");

        for (var widget = container.firstChild; widget; widget = widget.next) {

            if (widget.id == ev.guid) {

                container.removeChild(widget);
                break;
            }

        }

    }

    handleResourceAdded(ev: ToolCore.ResourceAddedEvent) {

        var db = ToolCore.getAssetDatabase();
        var asset = db.getAssetByGUID(ev.guid);

        var parent = asset.parent;
        var folderList = this.folderList;

        // these can be out of order
        if (asset.isFolder()) {

            if (!parent) {

                var id = folderList.addRootItem(asset.name, "Folder.icon", asset.guid);
                this.resourcesID = id;
                this.assetGUIDToItemID[asset.guid] = id;
                this.resourceFolder = asset;

            } else {
                var parentItemID = this.assetGUIDToItemID[parent.guid];
                var id = folderList.addChildItem(parentItemID, asset.name, "Folder.icon", asset.guid);
                this.assetGUIDToItemID[asset.guid] = id;
            }

        } else if (parent == this.currentFolder) {

            var container: Atomic.UILayout = <Atomic.UILayout>this.getWidget("contentcontainer");
            container.addChild(this.createButtonLayout(asset));

        }

    }

    handleWidgetEvent(data: Atomic.UIWidgetEvent): boolean {

        if (data.type == Atomic.UI_EVENT_TYPE_RIGHT_POINTER_UP) {

            var id = data.target.id;
            var db = ToolCore.getAssetDatabase();
            var asset: ToolCore.Asset;

            if (id == "folderList_")
                asset = db.getAssetByGUID(this.folderList.hoverItemID);
            else
                asset = db.getAssetByGUID(id);

            if (asset) {

                this.menu.createAssetContextMenu(this, asset, data.x, data.y);

                return true;

            }

        }

        if (data.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = data.target.id;

            // cancel search - goes back to the last selected folder
            if (id == "cancel search") {
                if (!ToolCore.toolSystem.project) return;
                this.searchEdit.text = "";
                this.refreshContent(this.currentFolder);
            }

            if (this.menu.handlePopupMenu(data.target, data.refid))
                return true;

            // create
            if (id == "menu create") {
                if (!ToolCore.toolSystem.project) return;
                var src = MenuItemSources.getMenuItemSource("project create items");
                var menu = new Atomic.UIMenuWindow(data.target, "create popup");
                menu.show(src);
                return true;

            }

            var db = ToolCore.getAssetDatabase();
            var fs = Atomic.getFileSystem();

            if (data.target && data.target.id.length) {

                if (id == "folderList_") {

                    var list = <Atomic.UISelectList>data.target;

                    var selectedId = list.selectedItemID;

                    // selectedId == 0 = root "Resources"

                    if (selectedId != "0") {

                        var asset = db.getAssetByGUID(selectedId);

                        if (asset.isFolder)
                            this.refreshContent(asset);
                    }

                    return true;

                }

                var asset = db.getAssetByGUID(id);

                if (asset) {

                    if (asset.isFolder()) {

                        this.folderList.selectItemByID(id);
                        this.refreshContent(asset);

                    } else {

                        this.sendEvent(EditorEvents.EditResource, { "path": asset.path });
                    }

                }

            }

            if (this.currentReferencedButton) {
                this.currentReferencedButton.setState(4, false);
                this.currentReferencedButton = null;
            }

        }

        return false;

    }

    rescan(asset: ToolCore.Asset) {

        var db = ToolCore.getAssetDatabase();
        db.scan();

    }

    selectPath(path: string) {

        var db = ToolCore.getAssetDatabase();

        var asset = db.getAssetByPath(path);

        if (!asset)
            return;

        this.folderList.selectItemByID(asset.guid);

    }

    handleFolderListSelectionChangedEvent(event: Atomic.UIListViewSelectionChangedEvent) {

        var selectedId = this.folderList.selectedItemID;

        if (selectedId != "0") {
            var db = ToolCore.getAssetDatabase();

            var asset = db.getAssetByGUID(selectedId);
            if (!asset)
                return;

            if (asset.isFolder)
                this.refreshContent(asset);
        }
    }

    handleDragEnded(data: Atomic.DragEndedEvent) {

        var asset: ToolCore.Asset;

        if (data.target) {

            var container: Atomic.UILayout = <Atomic.UILayout>this.getWidget("contentcontainer");

            if (data.target.id == "contentcontainerscroll" || container.isAncestorOf(data.target)) {

                if (data.target["asset"])
                    asset = <ToolCore.Asset>data.target["asset"];

                if (!asset || !asset.isFolder)
                    asset = this.currentFolder;
            }

        }

        if (!asset) {

            // if the drop target is the folderList's root select widget
            var rootList = this.folderList.rootList;
            var hoverID = rootList.hoverItemID;

            if (hoverID == "")
                return;

            var db = ToolCore.getAssetDatabase();
            asset = db.getAssetByGUID(hoverID);

        }

        if (!asset || !asset.isFolder)
            return;

        var dragObject = data.dragObject;

        if (dragObject.object && dragObject.object.typeName == "Node") {

            var node = <Atomic.Node>dragObject.object;

            var prefabComponent = <Atomic.PrefabComponent>node.getComponent("PrefabComponent");

            if (prefabComponent) {

                prefabComponent.savePrefab();

            }
            else {
                var destFilename = Atomic.addTrailingSlash(asset.path);
                destFilename += node.name + ".prefab";
                var file = new Atomic.File(destFilename, Atomic.FILE_WRITE);
                node.saveXML(file);
                file.close();
            }

            this.rescan(asset);

            return;

        } else if (dragObject.object && dragObject.object.typeName == "Asset") {

            var dragAsset = <ToolCore.Asset>dragObject.object;

            // get the folder we dragged on
            var destPath = Atomic.addTrailingSlash(asset.path);

            dragAsset.move(destPath + dragAsset.name + dragAsset.extension);

            this.refreshContent(this.currentFolder);

            return true;
        }

        // dropped some files?
        var filenames = dragObject.filenames;

        if (!filenames.length)
            return;

        var fileSystem = Atomic.getFileSystem();


        for (var i in filenames) {

            var srcFilename = filenames[i];

            var pathInfo = Atomic.splitPath(srcFilename);

            var destFilename = Atomic.addTrailingSlash(asset.path);

            destFilename += pathInfo.fileName + pathInfo.ext;

            fileSystem.copy(srcFilename, destFilename);

        }

        this.rescan(asset);

    }

    handleProjectLoaded(data) {

        this.folderList.rootList.value = 0;
        this.folderList.setExpanded(this.resourcesID, true);
        this.refreshContent(this.resourceFolder);

    }

    handleProjectUnloaded(data) {

        this.folderList.deleteAllItems();
        this.resourceFolder = null;

        var container: Atomic.UILayout = <Atomic.UILayout>this.getWidget("contentcontainer");
        container.deleteAllChildren();

    }

    // Shows referenced file in projectframe
    handleInspectorProjectReferenceHighlight(path: string): void {
        this.assetReferencePath = path;
        var db = ToolCore.getAssetDatabase();
        var asset = db.getAssetByPath(this.resourceFolder.getPath() + "/" + path);

        this.folderList.selectAllItems(false);
        this.folderList.selectItemByID(asset.parent.guid, true);
        this.refreshContent(asset.parent);
        this.folderList.scrollToSelectedItem();
    }

    // Searches folders within folders recursively
    searchProjectFolder(folderPath: string, container: Atomic.UILayout, searchText: string, db: ToolCore.AssetDatabase) {

        if (folderPath == "")
            return;

        var assets = db.getFolderAssets(folderPath);

        for (var i in assets) {

            var childAsset = assets[i];

            if (childAsset.isFolder()) {
                this.searchProjectFolder(childAsset.path, container, searchText, db);
            } else if (this.uiSearchBar.searchPopulate(searchText, childAsset.name + childAsset.extension)) {
                container.addChild(this.createButtonLayout(childAsset));
            }
        }
    }

    private refreshContent(folder: ToolCore.Asset) {

        if (this.currentFolder != folder) {

            this.sendEvent(EditorEvents.ContentFolderChanged, { path: folder.path });

        }

        this.currentFolder = folder;

        var db = ToolCore.getAssetDatabase();

        var container: Atomic.UILayout = <Atomic.UILayout>this.getWidget("contentcontainer");
        container.deleteAllChildren();

        if (this.currentFolder != null) {
            var assets = db.getFolderAssets(folder.path);

            this.containerScrollToHeightCounter = 0;

            if (this.searchEdit.text == "" || !this.search) {

                for (var i in assets) {
                    var asset = assets[i];
                    container.addChild(this.createButtonLayout(asset));
                    this.containerScrollToHeightCounter++;
                }
            } else if (this.search) {
                this.searchProjectFolder(this.resourceFolder.path, container, this.searchEdit.text, db);
            }
        }

        var containerScroll: Atomic.UIScrollContainer = <Atomic.UIScrollContainer>this.getWidget("contentcontainerscroll");
        containerScroll.scrollTo(0, this.containerScrollToHeight);
        this.search = false;
    }

    private createButtonLayout(asset: ToolCore.Asset): Atomic.UILayout {

        var system = ToolCore.getToolSystem();
        var project = system.project;
        var fs = Atomic.getFileSystem();

        var pathinfo = Atomic.splitPath(asset.path);

        var bitmapID = "Folder.icon";

        if (fs.fileExists(asset.path)) {
            bitmapID = "FileBitmap";
        }

        if (pathinfo.ext == ".js") {
            if (project.isComponentsDirOrFile(asset.path)) {
                bitmapID = "ComponentBitmap";
            }
            else {
                bitmapID = "JavascriptBitmap";
            }
        }

        var blayout = new Atomic.UILayout();
        blayout.id = asset.guid;

        blayout.gravity = Atomic.UI_GRAVITY_LEFT;

        var spacer = new Atomic.UIWidget();
        spacer.rect = [0, 0, 8, 8];
        blayout.addChild(spacer);

        var button = new Atomic.UIButton();



        // setup the drag object
        button.dragObject = new Atomic.UIDragObject(asset, asset.name);

        var lp = new Atomic.UILayoutParams;
        var buttonHeight = lp.height = 20;

        //Get the path of the button and compare it to the asset's path to highlight
        var resourcePath = this.resourceFolder.getPath() + "/" + this.assetReferencePath;

        //Highlight Button UI
        if (resourcePath == asset.path) {

            button.setState(4, true);
            this.currentReferencedButton = button;
            this.containerScrollToHeight = this.containerScrollToHeightCounter * buttonHeight;

        }

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        button.gravity = Atomic.UI_GRAVITY_LEFT;

        var image = new Atomic.UISkinImage(bitmapID);
        image.rect = [0, 0, 12, 12];
        image.gravity = Atomic.UI_GRAVITY_RIGHT;
        blayout.addChild(image);
        image["asset"] = asset;

        button.id = asset.guid;
        button.layoutParams = lp;
        button.fontDescription = fd;
        button.text = asset.name + asset.extension;
        button.skinBg = "TBButton.flat";
        button["asset"] = asset;
        blayout["assetButton"] = button;
        blayout.addChild(button);

        return blayout;
    }

}

export = ProjectFrame;
