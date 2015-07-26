
import ScriptWidget = require("../ScriptWidget");
import DataBinding = require("./DataBinding");
import InspectorUtils = require("./InspectorUtils");
import EditorUI = require("../EditorUI");

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

    inspect(component: Atomic.Component) {

        this.component = component;
        this.text = component.getTypeName();

        // don't expand by default
        this.value = 0;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var nlp = new Atomic.UILayoutParams();
        nlp.width = 304;

        // atttribute name layout param
        var atlp = new Atomic.UILayoutParams();
        atlp.width = 100;


        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        this.contentRoot.addChild(attrsVerticalLayout);

        var attrs = component.getAttributes();

        for (var i in attrs) {

            var attr = <Atomic.AttributeInfo> attrs[i];

            if (attr.mode & Atomic.AM_NOEDIT)
                continue;

            var binding = DataBinding.createBinding(component, attr);

            if (!binding)
                continue;

            var attrLayout = new Atomic.UILayout();

            attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

            var name = new Atomic.UITextField();
            name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            name.skinBg = "InspectorTextAttrName";
            name.layoutParams = atlp;

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
            name.fontDescription = fd;

            attrLayout.addChild(name);

            attrLayout.addChild(binding.widget);

            attrsVerticalLayout.addChild(attrLayout);

            this.bindings.push(binding);

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
            this.addJSComponentUI(attrsVerticalLayout);
        }

        if (component.typeName == "StaticModel" || component.typeName == "AnimatedModel" || component.typeName == "Skybox") {
            this.addModelUI(attrsVerticalLayout, component.typeName);
        }

        if (component.typeName == "SoundSource" || component.typeName == "SoundSource3D") {
            this.addSoundSourceUI(attrsVerticalLayout, component.typeName);
        }


        var deleteButton = new Atomic.UIButton();
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

    // Move these to a mixing class

    addPrefabUI(layout: Atomic.UILayout) {

        // expand prefab
        this.value = 1;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var selectButton = new Atomic.UIButton();
        selectButton.text = "Select Prefab";
        selectButton.fontDescription = fd;

        selectButton.onClick = () => {

            var node = (<Atomic.PrefabComponent> this.component).getPrefabNode();

            this.sendEvent("EditorActiveNodeChange", { node: node });

            return true;

        }

        layout.addChild(selectButton);

    }

    acceptAssetDrag(importerTypeName: string, ev: Atomic.DragEndedEvent): ToolCore.AssetImporter {

        var dragObject = ev.dragObject;

        if (dragObject.object && dragObject.object.typeName == "Asset") {

            var asset = <ToolCore.Asset> dragObject.object;

            if (asset.importerTypeName == importerTypeName) {
                return asset.importer;
            }

        }

        return null;

    }

    addModelUI(layout: Atomic.UILayout, typeName: string) {

        var staticModel = <Atomic.StaticModel> this.component;
        var cacheModel = staticModel.model;

        var o = InspectorUtils.createAttrEditFieldWithSelectButton("Model", layout);
        var field = o.editField;
        field.readOnly = true;

        var select = o.selectButton;

        select.onClick = () => {

            EditorUI.getModelOps().showResourceSelection("Select Model", "ModelImporter", function(asset: ToolCore.Asset) {

                staticModel.model = <Atomic.Model> Atomic.cache.getResource("Model", asset.cachePath + ".mdl");
                field.text = asset.name;

            });

        }

        if (cacheModel) {

            var asset = ToolCore.assetDatabase.getAssetByCachePath(cacheModel.name);

            if (asset) {

                field.text = asset.name;

            }

        }

        // handle dropping of model on field
        field.subscribeToEvent(field, "DragEnded", (ev: Atomic.DragEndedEvent) => {

            if (ev.target == field) {

                var importer = this.acceptAssetDrag("ModelImporter", ev);

                if (importer) {

                    var modelImporter = <ToolCore.ModelImporter> importer;
                    var asset = modelImporter.asset;

                    // the model itself, not the node XML
                    var model = <Atomic.Model> Atomic.cache.getResource("Model", asset.cachePath + ".mdl");

                    if (model) {

                        staticModel.model = model;
                        ev.target.text = asset.name;

                    }
                }
            }

        });

        // MATERIAL FIELD (single material, not multimaterial for now)

        o = InspectorUtils.createAttrEditFieldWithSelectButton("Material", layout);
        var materialField = o.editField;
        materialField.readOnly = true;

        select = o.selectButton;

        select.onClick = () => {

            EditorUI.getModelOps().showResourceSelection("Select Material", "MaterialImporter", function(asset: ToolCore.Asset) {

                staticModel.setMaterial(<Atomic.Material> Atomic.cache.getResource("Material", asset.path));

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

                    var materialImporter = <ToolCore.MaterialImporter> importer;
                    var asset = materialImporter.asset;

                    var material = <Atomic.Material> Atomic.cache.getResource("Material", asset.path);

                    if (material) {

                        staticModel.material = material;
                        ev.target.text = material.name;

                    }
                }
            }

        });


    }

    addJSComponentUI(layout: Atomic.UILayout) {

        var js = <Atomic.JSComponent> this.component;

        // expand prefab
        this.value = 1;

        var o = InspectorUtils.createAttrEditFieldWithSelectButton("Script", layout);
        var field = o.editField;
        field.readOnly = true;
        field.text = js.componentFile ? js.componentFile.name : "";

        var select = o.selectButton;

        select.onClick = () => {

            EditorUI.getModelOps().showResourceSelection("Select JSComponent Script", "JavascriptImporter", function(asset: ToolCore.Asset) {

                js.componentFile = <Atomic.JSComponentFile> Atomic.cache.getResource("JSComponentFile", asset.path);

                if (js.componentFile)
                    field.text = js.componentFile.name;

            });

        }

        // handle dropping of component on field
        field.subscribeToEvent(field, "DragEnded", (ev: Atomic.DragEndedEvent) => {

            if (ev.target == field) {

                var importer = this.acceptAssetDrag("JavascriptImporter", ev);

                if (importer) {

                    var jsImporter = <ToolCore.JavascriptImporter> importer;

                    if (jsImporter.isComponentFile()) {

                        js.componentFile = <Atomic.JSComponentFile> Atomic.cache.getResource("JSComponentFile", importer.asset.path);
                        if (js.componentFile)
                            ev.target.text = js.componentFile.name;

                    }

                }
            }

        });


    }


    addSoundSourceUI(layout: Atomic.UILayout, typeName: string) {

        var sndSource = <Atomic.SoundSource> this.component;

        var o = InspectorUtils.createAttrEditFieldWithSelectButton("Sound", layout);
        var field = o.editField;
        field.readOnly = true;
        field.text = sndSource.sound ? sndSource.sound.name : "";

        var select = o.selectButton;

        select.onClick = () => {

            EditorUI.getModelOps().showResourceSelection("Select Sound", "AudioImporter", function(asset: ToolCore.Asset) {

                sndSource.sound = <Atomic.Sound> Atomic.cache.getResource("Sound", asset.path);
                if (sndSource.sound)
                    field.text = sndSource.sound.name;

            });

        }

        // handle dropping of component on field
        field.subscribeToEvent(field, "DragEnded", (ev: Atomic.DragEndedEvent) => {

            if (ev.target == field) {

                var importer = this.acceptAssetDrag("AudioImporter", ev);

                if (importer) {

                    sndSource.sound = <Atomic.Sound> Atomic.cache.getResource("Sound", importer.asset.path);
                    if (sndSource.sound)
                        field.text = sndSource.sound.name;

                }
            }

        });


    }

    addLightCascadeParametersUI(layout: Atomic.UILayout) {

        var light = <Atomic.Light> this.component;

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

        var shape = <Atomic.CollisionShape> this.component;

        var button = new Atomic.UIButton();
        button.fontDescription = InspectorUtils.attrFontDesc;
        button.gravity = Atomic.UI_GRAVITY_RIGHT;
        button.text = "Set from Model";

        button.onClick = () => {

            var model = <Atomic.Drawable> shape.node.getComponent("StaticModel");
            if (model) {
                var box = model.boundingBox;
                shape.setBox([box[3] - box[0], box[4] - box[1], box[5] - box[2]]);
            }

        };

        layout.addChild(button);

    }


    component: Atomic.Component;
    bindings: Array<DataBinding> = new Array();


}

export = ComponentInspector;
