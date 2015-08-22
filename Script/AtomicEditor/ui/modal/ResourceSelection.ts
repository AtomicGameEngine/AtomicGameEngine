
import EditorUI = require("../EditorUI");
import EditorEvents = require("../../editor/EditorEvents");
import ModalWindow = require("./ModalWindow");

class ResourceSelection extends ModalWindow {

    folderList: Atomic.UIListView;
    callback: (asset: ToolCore.Asset, args:any) => void;
    args:any;

    populate(importerType: string) {

        var db = ToolCore.assetDatabase;
        var assets = db.getAssetsByImporterType(importerType);

        for (var i in assets) {

            var asset = assets[i];

            if (importerType == "JavascriptImporter") {
                if (!(<ToolCore.JavascriptImporter> asset.importer).isComponentFile())
                    continue;
            }

            this.folderList.addRootItem(asset.relativePath, "", asset.guid);

        }

    }

    constructor(windowText: string, importerType: string, callback: (asset: ToolCore.Asset, args:any) => void, args:any) {

        super();

        this.callback = callback;
        this.args = args;

        this.load("AtomicEditor/editor/ui/resourceselection.tb.txt");

        var foldercontainer = this.getWidget("resourcecontainer");

        var folderList = this.folderList = new Atomic.UIListView();

        folderList.rootList.id = "resourceList_";

        foldercontainer.addChild(folderList);

        this.populate(importerType);

        this.text = windowText;
        this.setSize(800, 600);
        this.center();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if(ev.count >= 2) {
            var id = ev.target.id;
            if(id == this.folderList.rootList.id) {
                this.selectFile();
            }
        }

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {
            var id = ev.target.id;

            if (id == "select") {

                return this.selectFile();

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    selectFile():boolean {
        var id = this.folderList.selectedItemID;
        if(id.length) {
            this.callback(ToolCore.assetDatabase.getAssetByGUID(id), this.args);
            this.hide();
            return true;
        }
        return false;
    }
}


export = ResourceSelection;
