//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("../EditorUI");
import EditorEvents = require("../../editor/EditorEvents");
import ModalWindow = require("./ModalWindow");

class ResourceSelection extends ModalWindow {

    folderList: Atomic.UIListView;
    callback: (returnObject: any, args: any) => void;
    args: any;
    resourceType: string;

    populate(importerType: string, resourceType: string) {

        var db = ToolCore.assetDatabase;
        var assets = db.getAssetsByImporterType(importerType, resourceType);

        this.folderList.addRootItem("None", "", "");

        for (var i in assets) {

            var asset = assets[i];

            if (importerType == "JavascriptImporter") {
                if (!(<ToolCore.JavascriptImporter>asset.importer).isComponentFile())
                    continue;
            }

            // TODO: Generalize this for other cache assets
            if (resourceType == "Animation") {

                var modelImporter = <ToolCore.ModelImporter>(asset.importer);
                var animations = modelImporter.getAnimations();

                if (animations.length) {

                    for (var i in animations) {

                        this.folderList.addRootItem(animations[i].animationName + " : " + asset.name, "", animations[i].name);

                    }


                }

            } else {

                this.folderList.addRootItem(asset.relativePath, "", asset.guid);

            }

        }

    }

    constructor(windowText: string, importerType: string, resourceType: string, callback: (asset: ToolCore.Asset, args: any) => void, args: any) {

        super();

        this.resourceType = resourceType;
        this.callback = callback;
        this.args = args;

        this.load("AtomicEditor/editor/ui/resourceselection.tb.txt");

        var foldercontainer = this.getWidget("resourcecontainer");

        var folderList = this.folderList = new Atomic.UIListView();

        folderList.rootList.id = "resourceList_";

        foldercontainer.addChild(folderList);

        this.populate(importerType, resourceType);

        this.text = windowText;
        this.setSize(800, 600);
        this.center();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.count >= 2) {
            var id = ev.target.id;
            if (id == this.folderList.rootList.id) {
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

    selectFile(): boolean {

        var id = this.folderList.selectedItemID;

        if (id == "") {
            this.sendEvent(EditorEvents.RemoveCurrentAssetAssigned);
            this.hide();
            return true;
        }

        if (this.resourceType == "Animation") {

          if (id.length) {
              this.callback(Atomic.cache.getResource("Animation", id), this.args);
              this.hide();
              return true;
          }

          this.hide();
          return true;

        } else {

            if (id.length) {
                this.callback(ToolCore.assetDatabase.getAssetByGUID(id), this.args);
                this.hide();
                return true;
            }
        }
        return false;
    }
}


export = ResourceSelection;
