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

import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");
import ResourceOps = require("resources/ResourceOps");

export class ResourceDelete extends ModalWindow {

    constructor(asset: ToolCore.Asset) {

        super();

        this.asset = asset;
        this.init("Delete Resource", "AtomicEditor/editor/ui/resourcedelete.tb.txt");
        var message = <Atomic.UIEditField>this.getWidget("message");

        var text = "Are you sure you want to delete resource:\n\n";
        text += asset.path;
        text += "\n\nThis operation cannot be undone";

        message.text = text;

        this.resizeToFitContent();
        this.center();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "delete") {

                this.hide();

                var db = ToolCore.getAssetDatabase();
                db.deleteAsset(this.asset);

                return true;
            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    asset: ToolCore.Asset;

}

export class CreateFolder extends ModalWindow {

    constructor(resourcePath: string) {

        super();

        this.resourcePath = resourcePath;
        this.init("New Folder", "AtomicEditor/editor/ui/resourcenewfolder.tb.txt");
        this.nameField = <Atomic.UIEditField>this.getWidget("folder_name");
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "create") {

                var resourcePath = Atomic.addTrailingSlash(this.resourcePath) + this.nameField.text;

                if (ResourceOps.CreateNewFolder(resourcePath)) {

                    this.hide();

                }

                return true;

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    resourcePath: string;
    nameField: Atomic.UIEditField;

}

export class CreateComponent extends ModalWindow {

    constructor(resourcePath: string) {

        super();

        this.resourcePath = resourcePath;
        this.init("New Component", "AtomicEditor/editor/ui/resourcecreatecomponent.tb.txt");
        this.nameField = <Atomic.UIEditField>this.getWidget("component_name");
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "create") {

                var componentName = this.nameField.text;
                var outputFile = Atomic.addTrailingSlash(this.resourcePath) + componentName;

                if (outputFile.indexOf(".js") == -1) outputFile += ".js";


                if (ResourceOps.CreateNewComponent(outputFile, componentName)) {

                    this.hide();

                    this.sendEvent(EditorEvents.EditResource, { path: outputFile });

                }

                return true;

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    resourcePath: string;
    nameField: Atomic.UIEditField;

}

export class CreateScript extends ModalWindow {

    constructor(resourcePath: string) {

        super();

        this.resourcePath = resourcePath;
        this.init("New Script", "AtomicEditor/editor/ui/resourcecreatecomponent.tb.txt");
        this.nameField = <Atomic.UIEditField>this.getWidget("component_name");
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "create") {

                var scriptName = this.nameField.text;
                var outputFile = Atomic.addTrailingSlash(this.resourcePath) + scriptName;

                if (outputFile.indexOf(".js") == -1) outputFile += ".js";


                if (ResourceOps.CreateNewScript(outputFile, scriptName)) {

                    this.hide();

                    this.sendEvent(EditorEvents.EditResource, { path: outputFile });

                }

                return true;

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    resourcePath: string;
    nameField: Atomic.UIEditField;

}

export class CreateScene extends ModalWindow {

    constructor(resourcePath: string) {

        super();

        this.resourcePath = resourcePath;
        this.init("New Scene", "AtomicEditor/editor/ui/resourcecreatecomponent.tb.txt");
        this.nameField = <Atomic.UIEditField>this.getWidget("component_name");
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "create") {

                var sceneName = this.nameField.text;
                var outputFile = Atomic.addTrailingSlash(this.resourcePath) + sceneName;

                if (outputFile.indexOf(".scene") == -1) outputFile += ".scene";

                if (ResourceOps.CreateNewScene(outputFile, sceneName)) {

                    this.hide();

                    this.sendEvent(EditorEvents.EditResource, { path: outputFile });

                }

                return true;

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    resourcePath: string;
    nameField: Atomic.UIEditField;

}

export class CreateMaterial extends ModalWindow {

    constructor(resourcePath: string) {

        super();

        this.resourcePath = resourcePath;
        this.init("New Material", "AtomicEditor/editor/ui/resourcecreatecomponent.tb.txt");
        this.nameField = <Atomic.UIEditField>this.getWidget("component_name");
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "create") {

                var materialName = this.nameField.text;
                var outputFile = Atomic.addTrailingSlash(this.resourcePath) + materialName;

                if (outputFile.indexOf(".material") == -1) outputFile += ".material";

                if (ResourceOps.CreateNewMaterial(outputFile, materialName)) {

                    this.hide();

                    this.sendEvent(EditorEvents.EditResource, { path: outputFile });

                }

                return true;

            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    resourcePath: string;
    nameField: Atomic.UIEditField;

}

export class RenameAsset extends ModalWindow {

    constructor(asset: ToolCore.Asset) {

        super();

        this.asset = asset;
        this.init("Rename Resource", "AtomicEditor/editor/ui/renameasset.tb.txt");

        var currentName = <Atomic.UITextField>this.getWidget("current_name");
        this.nameEdit = <Atomic.UIEditField>this.getWidget("new_name");

        currentName.text = asset.name;
        this.nameEdit.text = asset.name;

        this.resizeToFitContent();
        this.center();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "rename") {

                this.hide();

                if (this.asset.name != this.nameEdit.text)
                    this.asset.rename(this.nameEdit.text);

                return true;
            }

            if (id == "cancel") {

                this.hide();

                return true;
            }

        }

    }

    nameEdit: Atomic.UIEditField;
    asset: ToolCore.Asset;

}
