//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import ScriptWidget = require("ui/ScriptWidget");
import DataBinding = require("./DataBinding");
import InspectorUtils = require("./InspectorUtils");
import EditorUI = require("ui/EditorUI");
import CSComponentClassSelector = require("./CSComponentClassSelector");

class ComponentInspector extends Atomic.UISection {

    constructor() {

        super();

        this.subscribeToEvent("WidgetEvent", (data) => this.handleWidgetEvent(data));
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        var handled = false;

        for (var i = 0; i < this.bindings.length; i++) {

            if (this.bindings[i].handleWidgetEvent(ev)) {

                handled = true;

            }

        }

        // return if handled
        return handled;

    }

    addAttr(attr: Atomic.AttributeInfo, before: Atomic.UIWidget = null, after: Atomic.UIWidget = null): Atomic.UILayout {

        if (attr.mode & Atomic.AM_NOEDIT)
            return null;

        var binding = DataBinding.createBinding(this.component, attr);

        if (!binding)
            return null;

        var attrLayout = new Atomic.UILayout();

        attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";
        name.layoutParams = this.atlp;

        if (attr.type == Atomic.VAR_VECTOR3 || attr.type == Atomic.VAR_COLOR ||
            attr.type == Atomic.VAR_QUATERNION) {
            attrLayout.axis = Atomic.UI_AXIS_Y;
            attrLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
            attrLayout.skinBg = "InspectorVectorAttrLayout";
        }

        var bname = attr.name;

        if (bname == "Is Enabled")
            bname = "Enabled";

        name.text = bname;
        name.fontDescription = this.fd;

        attrLayout.addChild(name);

        attrLayout.addChild(binding.widget);

        if (before)
            this.attrsVerticalLayout.addChildBefore(attrLayout, before);
        else if (after)
            this.attrsVerticalLayout.addChildAfter(attrLayout, after);
        else
            this.attrsVerticalLayout.addChild(attrLayout);


        this.bindings.push(binding);

        return attrLayout;
    }

    inspect(component: Atomic.Component) {

        this.component = component;

        this.text = component.typeName;

        component.scene.sendEvent("SceneEditSerializable", { serializable: component, operation: 0});
        this.subscribeToEvent(component, "SceneEditSerializableUndoRedo", (data) => this.handleSceneEditSerializableUndoRedoEvent(data));

        // For JSComponents append the filename
        if (component.typeName == "JSComponent") {

            var jsc = <Atomic.JSComponent>component;

            if (jsc.componentFile) {
                var pathInfo = Atomic.splitPath(jsc.componentFile.name);
                this.text = "JS - " + pathInfo.fileName;
            }

        }

        // For CSComponents append the classname
        if (component.typeName == "CSComponent") {

            var csc = <AtomicNET.CSComponent>component;

            if (csc.componentClassName) {
                this.text = "CS - " + csc.componentClassName;
            }
        }

        // don't expand by default
        this.value = 0;

        var fd = this.fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var nlp = this.nlp = new Atomic.UILayoutParams();
        nlp.width = 304;

        // atttribute name layout param
        var atlp = this.atlp = new Atomic.UILayoutParams();
        atlp.width = 100;


        var attrsVerticalLayout = this.attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        this.contentRoot.addChild(attrsVerticalLayout);

        var attrs = component.getAttributes();

        for (var i in attrs) {
            this.addAttr(attrs[i]);
        }

        // custom component UI
        if (component.typeName == "PrefabComponent") {
            this.addPrefabUI(attrsVerticalLayout);
        }

        if (component.typeName == "Light") {
            this.addLightCascadeParametersUI(attrsVerticalLayout);
        }

        if (component.typeName == "CollisionShape") {
            this.addCollisionShapeUI(attrsVerticalLayout);
        }


        if (component.typeName == "JSComponent") {
            // auto expand JSComponents
            this.value = 1;
        }

        if (component.typeName == "CSComponent") {
            // auto expand CSComponents
            this.value = 1;
            this.addCSComponentUI(attrsVerticalLayout);

            var csc = <AtomicNET.CSComponent>this.component;
            var currentClassName = csc.componentClassName;

            this.subscribeToEvent(component, "CSComponentClassChanged", (ev: AtomicNET.CSComponentClassChangedEvent) => {

                if (currentClassName != ev.classname) {
                    //console.log("CSComponent Class Name Changed ", currentClassName, " ", ev.classname);
                    this.text = "CS - " + ev.classname;
                    currentClassName = ev.classname;
                    this.updateDataBindings();
                }

            });
        }


        if (component.typeName == "TileMap2D") {
            this.addTilemap2DUI(attrsVerticalLayout);
        }


        if (component.typeName == "StaticModel" || component.typeName == "AnimatedModel" || component.typeName == "Skybox") {
            this.addModelUI(attrsVerticalLayout, component.typeName);
        }

        if (component.typeName == "StaticSprite2D" || component.typeName == "AnimatedSprite2D") {
            this.addSpriteUI(attrsVerticalLayout, component.typeName);
        }


        var deleteButton = this.deleteButton = new Atomic.UIButton();
        deleteButton.text = "Delete Component";
        deleteButton.fontDescription = fd;

        deleteButton.onClick = () => {

            var node = this.component.node;
            this.component.remove();

            // refresh entire inspector, fix this...
            this.sendEvent("EditorActiveNodeChange", { node: node });
            
            return true;

        }

        attrsVerticalLayout.addChild(deleteButton);

        for (var i in this.bindings) {
            this.bindings[i].setWidgetValueFromObject();
            this.bindings[i].objectLocked = false;
        }

    }

    updateDataBindings() {

        var newBindings: Array<DataBinding> = new Array();
        var foundAttr: Array<Atomic.AttributeInfo> = new Array();

        var attrs = this.component.getAttributes();

        // run through current attr bindings looking for ones to preserve
        for (var i in this.bindings) {

            var binding = this.bindings[i];

            for (var j in attrs) {

                var attr = <Atomic.AttributeInfo>attrs[j];

                if (attr.name == binding.attrInfo.name && attr.type == binding.attrInfo.type) {

                    newBindings.push(binding);
                    foundAttr.push(attr);
                    break;

                }

            }

        }

        // remove bindings that no longer exist
        for (var i in this.bindings) {

            var binding = this.bindings[i];

            if (newBindings.indexOf(binding) == -1) {

                binding.widget.parent.remove();

            }

        }

        // set new bindings, additional bindings may be added below
        this.bindings = newBindings;

        // add new attr
        var curAttrLayout:Atomic.UILayout = null;
        for (var i in attrs) {

            var attr = attrs[i];

            if (foundAttr.indexOf(attr) == -1) {

                if (!curAttrLayout)
                  curAttrLayout = this.addAttr(attr, this.deleteButton);
                else
                  curAttrLayout = this.addAttr(attr, null, curAttrLayout);

            }

        }

        for (var i in this.bindings) {
            this.bindings[i].setWidgetValueFromObject();
            this.bindings[i].objectLocked = false;
        }

    }

    handleSceneEditSerializableUndoRedoEvent(ev) {

      for (var i in this.bindings) {
          this.bindings[i].objectLocked = true;
          this.bindings[i].setWidgetValueFromObject();
          this.bindings[i].objectLocked = false;
      }

    }

    // Move these to a mixing class

    addPrefabUI(layout: Atomic.UILayout) {

    }

    acceptAssetDrag(importerTypeName: string, ev: Atomic.DragEndedEvent): ToolCore.AssetImporter {

        var dragObject = ev.dragObject;

        if (dragObject.object && dragObject.object.typeName == "Asset") {

            var asset = <ToolCore.Asset>dragObject.object;

            if (asset.importerTypeName == importerTypeName) {
                return asset.importer;
            }

        }

        return null;

    }

    createMaterialClosure(layout: Atomic.UILayout, staticModel: Atomic.StaticModel, index: number) {

        var o = InspectorUtils.createAttrEditFieldWithSelectButton("Material " + index, layout);
        var materialField = o.editField;
        materialField.readOnly = true;

        var select = o.selectButton;

        select.onClick = () => {

            EditorUI.getModelOps().showResourceSelection("Select Material", "MaterialImporter", function(asset: ToolCore.Asset) {

                staticModel.setMaterialIndex(index, <Atomic.Material>Atomic.cache.getResource("Material", asset.path));

                if (staticModel.getMaterial())
                    materialField.text = staticModel.getMaterial().name;
                else
                    materialField.text = "";

            });

        }

        var material = staticModel.getMaterial();

        if (material) {

            materialField.text = material.name;

        }

        // handle dropping of material on field
        materialField.subscribeToEvent(materialField, "DragEnded", (ev: Atomic.DragEndedEvent) => {

            if (ev.target == materialField) {

                var importer = this.acceptAssetDrag("MaterialImporter", ev);

                if (importer) {

                    var materialImporter = <ToolCore.MaterialImporter>importer;
                    var asset = materialImporter.asset;

                    var material = <Atomic.Material>Atomic.cache.getResource("Material", asset.path);

                    if (material) {

                        staticModel.setMaterialIndex(index, material);
                        ev.target.text = material.name;

                    }
                }
            }

        });


    }

    addModelUI(layout: Atomic.UILayout, typeName: string) {

        var staticModel = <Atomic.StaticModel>this.component;

        var numGeometries = staticModel.numGeometries;
        if (typeName == "Skybox") {
            numGeometries = 1;
        }

        for (var x = 0; x < numGeometries; x++) {

            this.createMaterialClosure(layout, staticModel, x);

        }


    }


    addSpriteUI(layout: Atomic.UILayout, typeName: string) {

        var spriteComponent = <Atomic.StaticSprite2D>this.component;

        var o = InspectorUtils.createAttrEditFieldWithSelectButton("Sprite", layout);
        var field = o.editField;
        field.readOnly = true;
        field.text = spriteComponent.sprite ? spriteComponent.sprite.name : "";

        var select = o.selectButton;

        select.onClick = () => {

            // this should allow selecting of sprite sheets as well
            EditorUI.getModelOps().showResourceSelection("Select Sprite", "TextureImporter", function(asset: ToolCore.Asset) {

                spriteComponent.sprite = <Atomic.Sprite2D>Atomic.cache.getResource("Sprite2D", asset.path);
                if (spriteComponent.sprite)
                    field.text = spriteComponent.sprite.name;

            });

        }

        // handle dropping of component on field
        field.subscribeToEvent(field, "DragEnded", (ev: Atomic.DragEndedEvent) => {

            if (ev.target == field) {

                var importer = this.acceptAssetDrag("TextureImporter", ev);

                if (importer) {

                    spriteComponent.sprite = <Atomic.Sprite2D>Atomic.cache.getResource("Sprite2D", importer.asset.path);
                    if (spriteComponent.sprite)
                        field.text = spriteComponent.sprite.name;

                }
            }

        });


    }

    addTilemap2DUI(layout: Atomic.UILayout) {

        var tilemap = <Atomic.TileMap2D>this.component;

        var o = InspectorUtils.createAttrEditFieldWithSelectButton("TMX File", layout);
        var field = o.editField;
        field.readOnly = true;
        field.text = tilemap.tmxFile ? tilemap.tmxFile.name : "";

        var select = o.selectButton;

        select.onClick = () => {

            // this should allow selecting of sprite sheets as well
            EditorUI.getModelOps().showResourceSelection("Select TMX File", "TMXImporter", function(asset: ToolCore.Asset) {

                tilemap.tmxFile = <Atomic.TmxFile2D>Atomic.cache.getResource("TmxFile2D", asset.path);
                if (tilemap.tmxFile)
                    field.text = tilemap.tmxFile.name;
            });

        }

        // handle dropping of component on field
        field.subscribeToEvent(field, "DragEnded", (ev: Atomic.DragEndedEvent) => {

            if (ev.target == field) {

                var importer = this.acceptAssetDrag("TextureImporter", ev);

                if (importer) {

                    tilemap.tmxFile = <Atomic.TmxFile2D>Atomic.cache.getResource("TmxFile2D", importer.asset.path);
                    if (tilemap.tmxFile)
                        field.text = tilemap.tmxFile.name;

                }
            }

        });

    }

    addLightCascadeParametersUI(layout: Atomic.UILayout) {

        var light = <Atomic.Light>this.component;

        var cascadeInfo = light.getShadowCascade();

        var container = InspectorUtils.createContainer();
        container.gravity = Atomic.UI_GRAVITY_ALL;
        layout.addChild(container);

        var panel = new Atomic.UILayout();
        panel.axis = Atomic.UI_AXIS_Y;
        panel.layoutSize = Atomic.UI_LAYOUT_SIZE_PREFERRED;
        panel.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        container.addChild(panel);

        var label = InspectorUtils.createAttrName("CSM Splits:");
        panel.addChild(label);

        function createHandler(index, light, field) {

            return function(data: Atomic.UIWidgetEvent) {

                if (data.type == Atomic.UI_EVENT_TYPE_CHANGED) {

                    this.light.setShadowCascadeParameter(this.index, Number(this.field.text));

                }

            }.bind({ index: index, light: light, field: field });

        }

        var field = InspectorUtils.createAttrEditField("Split 0", panel);
        field.text = cascadeInfo[0].toString();
        field.subscribeToEvent(field, "WidgetEvent", createHandler(0, light, field));

        field = InspectorUtils.createAttrEditField("Split 1", panel);
        field.text = cascadeInfo[1].toString();
        field.subscribeToEvent(field, "WidgetEvent", createHandler(1, light, field));

        field = InspectorUtils.createAttrEditField("Split 2", panel);
        field.text = cascadeInfo[2].toString();
        field.subscribeToEvent(field, "WidgetEvent", createHandler(2, light, field));

        field = InspectorUtils.createAttrEditField("Split 3", panel);
        field.text = cascadeInfo[3].toString();
        field.subscribeToEvent(field, "WidgetEvent", createHandler(3, light, field));

    }


    addCollisionShapeUI(layout: Atomic.UILayout) {

        var shape = <Atomic.CollisionShape>this.component;

        var button = new Atomic.UIButton();
        button.fontDescription = InspectorUtils.attrFontDesc;
        button.gravity = Atomic.UI_GRAVITY_RIGHT;
        button.text = "Set from Model";

        button.onClick = () => {

            var model = <Atomic.Drawable>shape.node.getComponent("StaticModel");
            if (model) {
                var box = model.boundingBox;
                shape.setBox([box[3] - box[0], box[4] - box[1], box[5] - box[2]]);
            }

        };

        layout.addChild(button);

    }

    addCSComponentUI(layout: Atomic.UILayout) {

        for (var i in this.bindings) {

            var binding = this.bindings[i];

            // replace the Class widget with a editfield + select button
            if (binding.attrInfo.name == "Class") {

                var parent = binding.widget.parent;
                var text = binding.widget.text;
                binding.widget.parent.removeChild(binding.widget);
                var o = InspectorUtils.createAttrEditFieldWithSelectButton("", parent);
                o.editField.text = text;
                binding.widget = o.editField;

                o.selectButton.onClick = () => {

                    var cscomponent = <AtomicNET.CSComponent>this.component;
                    if (cscomponent.assemblyFile) {
                        var selector = new CSComponentClassSelector(o.editField, cscomponent);
                    }
                }

                break;
            }
        }
    }

    component: Atomic.Component;
    bindings: Array<DataBinding> = new Array();

    fd: Atomic.UIFontDescription;

    nlp: Atomic.UILayoutParams;

    // atttribute name layout param
    atlp: Atomic.UILayoutParams;

    attrsVerticalLayout: Atomic.UILayout;

    deleteButton: Atomic.UIButton;


}

export = ComponentInspector;
