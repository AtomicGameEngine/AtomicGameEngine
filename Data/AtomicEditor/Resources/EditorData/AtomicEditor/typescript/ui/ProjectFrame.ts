

import ScriptWidget = require("./ScriptWidget");
import Editor = require("../editor/Editor");

var UI = Atomic.UI;

class ProjectFrame extends ScriptWidget {

    folderList: Atomic.UIListView;

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/projectframe.tb.txt");

        this.gravity = UI.GRAVITY_TOP_BOTTOM;

        var projectviewcontainer = parent.getWidget("projectviewcontainer");

        projectviewcontainer.addChild(this);

        var foldercontainer = this.getWidget("foldercontainer");

        var folderList = this.folderList = new Atomic.UIListView();

        folderList.rootList.id = "folderList_";

        foldercontainer.addChild(folderList);

        // events
        this.subscribeToEvent("ProjectLoaded", (data) => this.handleProjectLoaded(data));

    }

    handleWidgetEvent(data): boolean {

        if (data.type == Atomic.UI.EVENT_TYPE_CLICK) {

            var db = ToolCore.getAssetDatabase();

            var fs = Atomic.getFileSystem();

            if (data.target && data.target.id.length) {

                var id = data.target.id;

                if (id == "folderList_") {

                    var list = <Atomic.UISelectList> data.target;

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

                    this.sendEvent("EditResource", { "path": asset.path });

                  }

                }

            }

        }

        return false;

    }


    handleProjectLoaded(data) {

        this.refresh();

    }

    private refreshContent(folder:ToolCore.Asset) {

        var db = ToolCore.getAssetDatabase();

        var container: Atomic.UILayout = <Atomic.UILayout> this.getWidget("contentcontainer");
        container.deleteAllChildren();

        var assets = db.getFolderAssets(folder.path);

        for (var i in assets) {

            var asset = assets[i];

            container.addChild(this.createButtonLayout(asset));
        }

    }

    private recursiveAddFolder(parentItemID: number, folder: string) {

        var db = ToolCore.getAssetDatabase();

        var folderList = this.folderList;

        var assets = db.getFolderAssets(folder);

        for (var i in assets) {

            var asset = assets[i];

            if (asset.isFolder()) {

                var childItemID = folderList.addChildItem(parentItemID, asset.name, "Folder.icon", asset.guid);

                this.recursiveAddFolder(childItemID, asset.path);

            }

        }

    }


    refresh() {

        var system = ToolCore.getToolSystem();
        var project = system.project;

        var folderList = this.folderList;

        folderList.deleteAllItems();

        var resourcesID = folderList.addRootItem("Resources", "Folder.icon", "0");

        this.recursiveAddFolder(resourcesID, project.resourcePath);

        folderList.setExpanded(resourcesID, true);
        // this.refreshContent(project.resourcePath);
        folderList.rootList.value = 0;


    }

    private createButtonLayout(asset:ToolCore.Asset): Atomic.UILayout {

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

        blayout.gravity = UI.GRAVITY_LEFT;

        var spacer = new Atomic.UIWidget();
        spacer.rect = [0, 0, 8, 8];
        blayout.addChild(spacer);

        var button = new Atomic.UIButton();

        var lp = new Atomic.UILayoutParams;
        lp.height = 20;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        button.gravity = UI.GRAVITY_LEFT;

        var image = new Atomic.UISkinImage(bitmapID);
        image.rect = [0, 0, 12, 12];
        image.gravity = UI.GRAVITY_RIGHT;
        blayout.addChild(image);

        button.id = asset.guid;
        button.layoutParams = lp;
        button.fontDescription = fd;
        button.text = asset.name;
        button.skinBg = "TBButton.flat";
        blayout.addChild(button);

        return blayout;
    }

}

export = ProjectFrame;
