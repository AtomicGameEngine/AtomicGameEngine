//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("ui/EditorUI");
import InspectorUtils = require("./InspectorUtils");
import AttributeInfoEdit = require("./AttributeInfoEdit");
import SerializableEditType = require("./SerializableEditType");

class LightCascadeAttributeEdit extends AttributeInfoEdit {

    splitFields: Atomic.UIEditField[] = [];

    createEditWidget() {

        var panel = new Atomic.UILayout();
        panel.axis = Atomic.UI_AXIS_Y;
        panel.layoutSize = Atomic.UI_LAYOUT_SIZE_PREFERRED;
        panel.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;

        var lp = new Atomic.UILayoutParams();
        lp.width = 180;
        panel.layoutParams = lp;

        this.editWidget = panel;

        var _this = this;

        function createHandler(index, field) {

            return function(ev: Atomic.UIWidgetEditCompleteEvent) {

                for (var i in _this.editType.objects) {

                    var o = <Atomic.Light>_this.editType.objects[i];
                    o.setShadowCascadeParameter(this.index, Number(this.field.text));

                }

                o.scene.sendEvent("SceneEditEnd");
                _this.refresh();

            }.bind({ index: index, field: field });

        }

        var flp = new Atomic.UILayoutParams();
        flp.width = 60;

        for (var i = 0; i < 4; i++) {
            var field = InspectorUtils.createAttrEditField("Split " + i, panel);
            field.layoutParams = flp;
            field.subscribeToEvent(field, "UIWidgetEditComplete", createHandler(i, field));
            this.splitFields.push(field);
        }

    }

    refresh() {

        // Vector 4 internally
        for (var i = 0; i < 4; i++) {

            var uniform = this.editType.getUniformValue(this.attrInfo, i);

            var field = this.splitFields[i];

            if (uniform) {

                var object = this.editType.getFirstObject();

                if (object) {
                    var value = object.getAttribute(this.attrInfo.name);
                    field.text = parseFloat(value[i].toFixed(5)).toString();
                }
                else {

                    field.text = "???";
                }
            }
            else {
                field.text = "--";
            }

        }


    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

            return true;
        }

        return false;

    }

}

interface MaterialEdit {

    index: number;
    editField: Atomic.UIEditField;
    selectButton: Atomic.UIButton;

}

class SubmeshAttributeEdit extends AttributeInfoEdit {

    materialIndexes: number[] = [];

    materialEdits: { [index: number]: MaterialEdit } = {};

    mainLayout: Atomic.UILayout;
    enabledCheckBox: Atomic.UICheckBox;
    nameField: Atomic.UITextField;
    name: string;

    constructor(name: string) {

        super();
        this.name = name;
        this.hideName = true;
    }

    createMaterialEdit(materialIndex: number) {

        var o = InspectorUtils.createAttrEditFieldWithSelectButton("Material", this.mainLayout);

        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        lp.height = 24;
        o.editField.layoutParams = lp;
        o.editField.readOnly = true;

        var selectButton = o.selectButton;

        var materialEdit: MaterialEdit = { index: materialIndex, editField: o.editField, selectButton: selectButton };
        this.materialEdits[materialIndex] = materialEdit;

        var resourceTypeName = "Material";
        var importerName = ToolCore.assetDatabase.getResourceImporterName(resourceTypeName);

        selectButton.onClick = () => {

            EditorUI.getModelOps().showResourceSelection("Select " + resourceTypeName + " Resource", importerName, resourceTypeName, function(retObject: any) {

                var resource: Atomic.Resource = null;

                if (retObject instanceof ToolCore.Asset) {

                    resource = (<ToolCore.Asset>retObject).getResource(resourceTypeName);

                } else if (retObject instanceof Atomic.Resource) {

                    resource = <Atomic.Resource>retObject;

                }

                this.editType.onAttributeInfoEdited(this.attrInfo, resource, materialIndex);
                this.refresh();

            }.bind(this));
        }

        // handle dropping of component on field
        o.editField.subscribeToEvent(o.editField, "DragEnded", (ev: Atomic.DragEndedEvent) => {

            if (ev.target == o.editField) {

                var dragObject = ev.dragObject;

                var importer;

                if (dragObject.object && dragObject.object.typeName == "Asset") {

                    var asset = <ToolCore.Asset>dragObject.object;

                    if (asset.importerTypeName == importerName) {
                        importer = asset.importer;
                    }

                }

                if (importer) {

                    var resource = asset.getResource(resourceTypeName);

                    this.editType.onAttributeInfoEdited(this.attrInfo, resource, materialIndex);
                    this.refresh();
                }
            }

        });

    }

    createEditWidget() {

        var mainLayout = this.mainLayout = new Atomic.UILayout();
        mainLayout.axis = Atomic.UI_AXIS_Y;
        mainLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;
        mainLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        mainLayout.gravity = Atomic.UI_GRAVITY_LEFT_RIGHT;
        mainLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var cb = InspectorUtils.createAttrCheckBox(this.name, mainLayout);
        this.enabledCheckBox = cb.checkBox;

        cb.checkBox.subscribeToEvent(cb.checkBox, "WidgetEvent", (ev: Atomic.UIWidgetEvent) => {

            if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

                var scene: Atomic.Scene;

                for (var i in this.editType.objects) {

                    var staticModel = <Atomic.StaticModel>this.editType.objects[i];

                    scene = staticModel.scene;

                    if (cb.checkBox.value) {

                        staticModel.showGeometry(this.name);

                    } else {

                        staticModel.hideGeometry(this.name);

                    }

                }

                scene.sendEvent("SceneEditEnd");

                return true;

            }

            return false;


        });


        this.nameField = cb.textField;

        this.editWidget = mainLayout;
    }

    refresh() {

        var editType = this.editType;

        var object = this.editType.getFirstObject();

        if (!object) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        this.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;

        var enabled = (<Atomic.StaticModel>object).getGeometryVisible(this.name);
        var mixed = false;
        for (var i in editType.objects) {

            object = editType.objects[i];
            var _enabled = (<Atomic.StaticModel>object).getGeometryVisible(this.name);
            if (_enabled != enabled) {
                mixed = true;
                break;
            }
        }

        if (mixed) {

            this.enabledCheckBox.skinBg = "TBGreyCheckBoxNonUniform";
            this.enabledCheckBox.value = 1;

        } else {

            this.enabledCheckBox.skinBg = "TBGreyCheckBox";
            this.enabledCheckBox.value = enabled ? 1 : 0;

        }

        for (var i in this.materialIndexes) {

            var idx = this.materialIndexes[i];

            if (!this.materialEdits[idx]) {
                this.createMaterialEdit(idx);
            }

            var matEdit = this.materialEdits[idx];

            var uniform = editType.getUniformValue(this.attrInfo, matEdit.index);

            if (uniform) {

                var object = editType.getFirstObject();

                if (object) {

                    // for cached resources, use the asset name, otherwise use the resource path name
                    var resource: Atomic.Resource;
                    if (matEdit.index != -1) {
                        resource = object.getAttribute(this.attrInfo.name).resources[matEdit.index];
                    } else {
                        resource = <Atomic.Resource>object.getAttribute(this.attrInfo.name);
                    }

                    var text = "";

                    if (resource) {

                        if (resource instanceof Atomic.Material) {

                            text = (<Atomic.Material>resource).name;

                        } else {
                            text = "???";
                        }
                    }

                    var pathinfo = Atomic.splitPath(text);
                    matEdit.editField.text = pathinfo.fileName;
                }


            } else {
                matEdit.editField.text = "--";
            }

        }

    }

}

class SubmeshListAttributeEdit extends AttributeInfoEdit {

    layout: Atomic.UILayout;

    submeshEdits: { [name: string]: SubmeshAttributeEdit } = {};

    constructor() {

        super();

        this.hideName = true;

    }

    createEditWidget() {

        this.spacing = 0;

        var layout = this.layout = new Atomic.UILayout();

        layout.axis = Atomic.UI_AXIS_Y;
        layout.spacing = 2;
        layout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;
        layout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        layout.gravity = Atomic.UI_GRAVITY_LEFT_RIGHT;
        layout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var lp = new Atomic.UILayoutParams();
        lp.width = 304;
        layout.layoutParams = lp;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";
        name.layoutParams = AttributeInfoEdit.attrNameLP;
        name.text = "Submeshes";

        layout.addChild(name);

        InspectorUtils.createSeparator(layout);

        this.editWidget = layout;

    }

    refresh() {

        var editType = this.editType;

        var object = this.editType.getFirstObject();

        if (!object) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        this.visibility = Atomic.UI_WIDGET_VISIBILITY_VISIBLE;

        var name: string;
        for (var i in editType.objects) {

            // TODO: check for multiple selection with different models and display multiple selection info box

            var staticModel = <Atomic.StaticModel>editType.objects[i];
            var model = staticModel.model;

            if (!model)
                continue;

            for (var j = 0; j < model.numGeometries; j++) {

                name = model.getGeometryName(j);

                if (!name)
                    name = "Mesh";

                if (!this.submeshEdits.hasOwnProperty(name)) {

                    var submeshEdit = new SubmeshAttributeEdit(name);
                    submeshEdit.initialize(this.editType, this.attrInfo);
                    this.layout.addChild(submeshEdit);
                    this.submeshEdits[name] = submeshEdit;
                    InspectorUtils.createSeparator(this.layout);
                }

                this.submeshEdits[name].materialIndexes.push(j);
            }

        }

        for (name in this.submeshEdits)
            this.submeshEdits[name].refresh();

    }

}

AttributeInfoEdit.registerCustomAttr("AnimatedModel", "Material", SubmeshListAttributeEdit);
AttributeInfoEdit.registerCustomAttr("StaticModel", "Material", SubmeshListAttributeEdit);
AttributeInfoEdit.registerCustomAttr("Light", "CSM Splits", LightCascadeAttributeEdit);
