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

import EditorUI = require("ui/EditorUI");
import InspectorUtils = require("./InspectorUtils");
import SerializableEditType = require("./SerializableEditType");
import ColorChooser = require("./ColorChooser");

class AttributeInfoEdit extends Atomic.UILayout {

    attrInfo: Atomic.AttributeInfo;
    editType: SerializableEditType;

    editWidget: Atomic.UIWidget;

    nameOverride: string;
    hideName: boolean = false;

    arrayIndex:number = -1;

    constructor() {

        super();

    }

    initialize(editType: SerializableEditType, attrInfo: Atomic.AttributeInfo): boolean {

        this.editType = editType;
        this.attrInfo = attrInfo;

        this.createLayout();

        return true;

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        return false;

    }

    createLayout() {

        this.createEditWidget();

        this.editWidget.subscribeToEvent(this.editWidget, Atomic.UIWidgetEvent((data) => this.handleWidgetEvent(data)));

        if (this.attrInfo.tooltip) {
            this.editWidget.tooltip = this.attrInfo.tooltip;
        }

        var attr = this.attrInfo;
        var attrNameLP = AttributeInfoEdit.attrNameLP;

        this.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        if (attr.type == Atomic.VariantType.VAR_VECTOR3  ||
            attr.type == Atomic.VariantType.VAR_QUATERNION) {
            this.axis = Atomic.UI_AXIS.UI_AXIS_Y;
            this.layoutPosition = Atomic.UI_LAYOUT_POSITION.UI_LAYOUT_POSITION_LEFT_TOP;
            this.skinBg = "InspectorVectorAttrLayout";
        }

        if (!this.hideName) {

            var name = new Atomic.UITextField();
            name.textAlign = Atomic.UI_TEXT_ALIGN.UI_TEXT_ALIGN_LEFT;
            name.skinBg = "InspectorTextAttrName";
            name.layoutParams = attrNameLP;
            var bname = attr.name;

            if (bname == "Is Enabled")
                bname = "Enabled";

            if (this.nameOverride)
                name.text = this.nameOverride;
            else
                name.text = bname;

            name.fontDescription = AttributeInfoEdit.fontDesc;

            if (this.attrInfo.tooltip) {
               name.tooltip = this.attrInfo.tooltip;
            }

            this.addChild(name);


        }

        this.addChild(this.editWidget);

    }

    createEditWidget() {

    }

    refresh() {


    }

    static createAttrEdit(editType: SerializableEditType, attrInfo: Atomic.AttributeInfo, nameOverride:string = undefined, typeOverride:Atomic.VariantType = undefined ): AttributeInfoEdit {

        var type: typeof AttributeInfoEdit;
        var customTypes = AttributeInfoEdit.customAttrEditTypes[editType.typeName];
        if (customTypes) {

            type = customTypes[attrInfo.name];

        }

        if (typeOverride)
            type = AttributeInfoEdit.standardAttrEditTypes[typeOverride];

        if (!type && attrInfo.isArray) {
            type = ArrayAttributeEdit;
        }

        if (!type) {
            type = AttributeInfoEdit.standardAttrEditTypes[attrInfo.type];
        }

        if (!type)
            return null;

        var attrEdit = new type();
        attrEdit.nameOverride = nameOverride;
        if (!attrEdit.initialize(editType, attrInfo))
            return null;

        return attrEdit;

    }

    // atttribute name layout param
    static attrNameLP: Atomic.UILayoutParams;
    static fontDesc: Atomic.UIFontDescription;

    static standardAttrEditTypes: { [variantType: number /*Atomic.VariantType*/]: typeof AttributeInfoEdit } = {};

    static customAttrEditTypes: { [typeName: string]: { [name: string]: typeof AttributeInfoEdit } } = {};

    static registerCustomAttr(typeName: string, attrName: string, edit: typeof AttributeInfoEdit) {

        if (!AttributeInfoEdit.customAttrEditTypes[typeName]) {
            AttributeInfoEdit.customAttrEditTypes[typeName] = {};
        }

        AttributeInfoEdit.customAttrEditTypes[typeName][attrName] = edit;

    }

    private static Ctor = (() => {

        var attrNameLP = AttributeInfoEdit.attrNameLP = new Atomic.UILayoutParams();
        attrNameLP.width = 120;

        var fd = AttributeInfoEdit.fontDesc = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

    })();

}

class BoolAttributeEdit extends AttributeInfoEdit {

    createEditWidget() {

        var box = new Atomic.UICheckBox();
        this.editWidget = box;
    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo);

        if (uniform) {
            var object = this.editType.getFirstObject();
            this.editWidget.skinBg = "TBCheckBox";
            if (object) {
                var value = object.getAttribute(this.attrInfo.name, this.arrayIndex);
                this.editWidget.value = (value ? 1 : 0);
            }

        } else {

            this.editWidget.skinBg = "TBCheckBoxNonUniform";
            this.editWidget.value = 1;

        }

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CHANGED) {

            this.editType.onAttributeInfoEdited(this.attrInfo, this.editWidget.value ? true : false, -1, true, this.arrayIndex);
            this.refresh();

            return true;
        }

        return false;

    }

}

class StringAttributeEdit extends AttributeInfoEdit {

    createEditWidget() {

        var field = new Atomic.UIEditField();
        field.textAlign = Atomic.UI_TEXT_ALIGN.UI_TEXT_ALIGN_LEFT;
        field.skinBg = "TBAttrEditorField";
        field.fontDescription = AttributeInfoEdit.fontDesc;
        var lp = new Atomic.UILayoutParams();
        lp.width = 160;
        field.layoutParams = lp;

        field.subscribeToEvent(field, Atomic.UIWidgetEditCompleteEvent((ev) => this.handleUIWidgetEditCompleteEvent(ev)));

        this.editWidget = field;
    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo);

        if (uniform) {
            var object = this.editType.getFirstObject();
            if (object) {
                var value = object.getAttribute(this.attrInfo.name, this.arrayIndex);
                this.editWidget.text = value;
            }

        } else {

            this.editWidget.text = "--";

        }

    }

    handleUIWidgetEditCompleteEvent(ev) {

        this.editType.onAttributeInfoEdited(this.attrInfo, this.editWidget.text, -1, true, this.arrayIndex);
        this.refresh();

    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CHANGED) {

            return true;
        }

        return false;

    }

}

class IntAttributeEdit extends AttributeInfoEdit {

    enumSource: Atomic.UISelectItemSource;

    createEditWidget() {

        var attrInfo = this.attrInfo;

        if (attrInfo.enumNames.length) {

            var enumSource = this.enumSource = new Atomic.UISelectItemSource();

            for (var i in attrInfo.enumNames) {

                enumSource.addItem(new Atomic.UISelectItem(attrInfo.enumNames[i], (Number(i) + 1).toString()));

            }

            var button = new Atomic.UIButton();
            button.fontDescription = AttributeInfoEdit.fontDesc;
            button.text = "Enum Value!";
            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            button.layoutParams = lp;

            this.editWidget = button;

        } else {


            var field = new Atomic.UIEditField();
            field.textAlign = Atomic.UI_TEXT_ALIGN.UI_TEXT_ALIGN_CENTER;
            field.skinBg = "TBAttrEditorField";
            field.fontDescription = AttributeInfoEdit.fontDesc;
            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            field.layoutParams = lp;

            field.subscribeToEvent(field, Atomic.UIWidgetEditCompleteEvent((ev) => this.handleUIWidgetEditCompleteEvent(ev)));

            this.editWidget = field;
        }
    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo);

        if (uniform) {
            var object = this.editType.getFirstObject();
            if (object) {

                var value = object.getAttribute(this.attrInfo.name, this.arrayIndex);
                var widget = this.editWidget;
                var attrInfo = this.attrInfo;

                if (attrInfo.enumNames.length) {

                    if (attrInfo.enumValues.indexOf(value) != -1) {
                        widget.text = attrInfo.enumNames[attrInfo.enumValues.indexOf(value)];
                    } else {
                        widget.text = attrInfo.enumNames[value];
                    }
                
                }
                else {
                    widget.text = value.toString();
                }
            }

        } else {

            this.editWidget.text = "--";

        }

    }

    handleUIWidgetEditCompleteEvent(ev) {

        // non-enum
        this.editType.onAttributeInfoEdited(this.attrInfo, Number(this.editWidget.text), -1, true, this.arrayIndex);
        this.refresh();

    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CHANGED) {

            return true;
        }

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {

            var id = this.attrInfo.name + " enum popup";

            if (ev.target.id == id) {
                let eindex =  Number(ev.refid) - 1;
                let value = this.attrInfo.enumValues[eindex];
                this.editType.onAttributeInfoEdited(this.attrInfo, value, -1, true, this.arrayIndex);
                this.refresh();
            }

            else if (this.editWidget == ev.target && this.attrInfo.enumNames.length) {


                if (this.enumSource) {
                    var menu = new Atomic.UIMenuWindow(ev.target, id);
                    menu.show(this.enumSource);
                }

                return true;

            }

        }

        return false;

    }

}

class FloatAttributeEdit extends AttributeInfoEdit {

    createEditWidget() {

        var attrInfo = this.attrInfo;

        var field = new Atomic.UIEditField();
        field.textAlign = Atomic.UI_TEXT_ALIGN.UI_TEXT_ALIGN_CENTER;
        field.skinBg = "TBAttrEditorField";
        field.fontDescription = AttributeInfoEdit.fontDesc;
        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        field.layoutParams = lp;

        field.subscribeToEvent(field, Atomic.UIWidgetEditCompleteEvent((ev) => this.handleUIWidgetEditCompleteEvent(ev)));

        this.editWidget = field;

    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo);

        if (uniform) {
            var object = this.editType.getFirstObject();
            if (object) {

                var widget = this.editWidget;
                var attrInfo = this.attrInfo;
                var value = object.getAttribute(attrInfo.name, this.arrayIndex);

                if (value == undefined) {

                    console.log("WARNING: Undefined value for object: ", this.editType.typeName + "." + attrInfo.name);
                    widget.text = "???";

                } else {
                    widget.text = parseFloat(value.toFixed(5)).toString();
                }

            }

        } else {

            this.editWidget.text = "--";

        }

    }

    handleUIWidgetEditCompleteEvent(ev) {

        this.editType.onAttributeInfoEdited(this.attrInfo, Number(this.editWidget.text), -1, true, this.arrayIndex);
        this.refresh();

    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CHANGED) {

            return true;
        }

        return false;

    }

}

class NumberArrayAttributeEdit extends AttributeInfoEdit {

    selects: Atomic.UIInlineSelect[] = [];

    private numElements: number;

    constructor(numElements: number) {

        super();

        this.numElements = numElements;

    }

    createEditWidget() {

        var attrInfo = this.attrInfo;

        var layout = new Atomic.UILayout();
        layout.spacing = 0;

        var lp = new Atomic.UILayoutParams();
        lp.width = this.numElements != 4 ? 100 : 70;

        for (var i = 0; i < this.numElements; i++) {

            var select = new Atomic.UIInlineSelect();
            this.selects.push(select);

            select.id = String(i + 1);
            select.fontDescription = AttributeInfoEdit.fontDesc;
            select.skinBg = "InspectorVectorAttrName";
            select.setLimits(-10000000, 10000000);
            if (this.numElements != 4) {
                var editlp = new Atomic.UILayoutParams();
                editlp.minWidth = 60;
                select.editFieldLayoutParams = editlp;
            }
            select.layoutParams = lp;
            layout.addChild(select);

            select["_edit"] = select.getWidget("edit");
            select["_dec"] = select.getWidget("dec");
            select["_inc"] = select.getWidget("inc");

            select.subscribeToEvent(select, Atomic.UIWidgetEvent((ev) => this.handleWidgetEvent(ev)));
            select.subscribeToEvent(select, Atomic.UIWidgetEditCompleteEvent((ev) => this.handleUIWidgetEditCompleteEvent(ev)));
        }

        this.editWidget = layout;

    }

    refresh() {

        for (var i = 0; i < this.selects.length; i++) {

            var select = this.selects[i];
            if (select["_edit"].focus || select["_dec"].captured || select["_inc"].captured)
                continue;

            var uniform = this.editType.getUniformValue(this.attrInfo, i);

            if (uniform) {

                var object = this.editType.getFirstObject();

                if (object) {

                    var value = object.getAttribute(this.attrInfo.name, this.arrayIndex);
                    select.value = parseFloat(value[i].toFixed(5));

                }

            } else {

                select["_edit"].text = "--";

            }

        }


    }

    handleUIWidgetEditCompleteEvent(ev: Atomic.UIWidgetEditCompleteEvent) {

        var index = Number(ev.widget.id) - 1;
        this.editType.onAttributeInfoEdited(this.attrInfo, ev.widget.value, index, true, this.arrayIndex);
        this.refresh();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CHANGED) {

            var captured = false;
            for (var i in this.selects) {
                var select = this.selects[i];
                if (select["_dec"].captured || select["_inc"].captured) {

                    captured = true;
                    break;

                }
            }

            if (captured) {

                var index = Number(ev.target.id) - 1;
                this.editType.onAttributeInfoEdited(this.attrInfo, ev.target.value, index, false, this.arrayIndex);

            }

            return true;
        }

        return false;

    }

}

class Vector2AttributeEdit extends NumberArrayAttributeEdit {

    constructor() {

        super(2);

    }

}


class Vector3AttributeEdit extends NumberArrayAttributeEdit {

    constructor() {

        super(3);

    }

}

class QuaternionAttributeEdit extends NumberArrayAttributeEdit {

    constructor() {

        super(3);

    }

}

class ColorAttributeEdit extends AttributeInfoEdit {


    createLayout() {

        var layout = new Atomic.UILayout();
        let name = this.attrInfo.name;
        if (this.nameOverride) {
            name = this.nameOverride;
        }
        var o = InspectorUtils.createAttrColorFieldWithSelectButton(name, layout);

        var colorWidget = this.colorWidget = o.colorWidget;
        var selectButton = o.selectButton;

        layout.layoutSize = Atomic.UI_LAYOUT_SIZE.UI_LAYOUT_SIZE_AVAILABLE;
        layout.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_LEFT_RIGHT;
        layout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        lp.height = 24;
        colorWidget.layoutParams = lp;

        this.editWidget = layout;

        this.editWidget.subscribeToEvent(this.editWidget, Atomic.UIWidgetEvent((data) => this.handleWidgetEvent(data)));

        selectButton.onClick = () => {

            // store original color
            let color = [1, 1, 1, 1];
            let object = this.editType.getFirstObject();

            if (object) {
                color = object.getAttribute(this.attrInfo.name, this.arrayIndex);
            }

            colorWidget.color = color;

            let restore = null;
            let chooser = new ColorChooser ( color );

            this.subscribeToEvent(chooser, Editor.ColorChooserChangedEvent((ev) => {

                restore = color;
                this.updateColor(chooser.getRGBA());

            }));

            this.subscribeToEvent(chooser, Atomic.UIWidgetEditCanceledEvent((ev) => {

                if (restore) {

                    colorWidget.color = restore;
                    this.updateColor(restore);

                }

            }));

            this.subscribeToEvent(chooser, Atomic.UIWidgetEditCompleteEvent((ev) => {

                let newColor = chooser.getRGBA();

                // check for new color edit
                let committed = false;
                for (let i = 0; i < 4; i++) {

                    if (color[i] != newColor[i]) {

                        this.editType.onAttributeInfoEdited(this.attrInfo, newColor, -1, true, this.arrayIndex);
                        this.refresh();
                        committed = true;
                        break;

                    }
                }

                if (restore && !committed) {

                    for (let i = 0; i < 4; i++) {

                        if (color[i] != restore[i]) {

                            this.updateColor(color);
                            break;

                        }

                    }

                }

            }));

        };

        this.addChild(this.editWidget);

    }

    refresh() {

        let object = this.editType.getFirstObject();

        if (object) {
            let color = object.getAttribute(this.attrInfo.name, this.arrayIndex);
            this.colorWidget.color = color;
        }


    }

    // updates color on selection without committing to undo/redo for preview
    updateColor(rgba:number[]) {

        this.colorWidget.color = rgba;

        for (var i in this.editType.objects) {

            let object = this.editType.objects[i];
            object.setAttribute(this.attrInfo.name, rgba, this.arrayIndex);

        }

    }

    colorWidget : Atomic.UIColorWidget;

}

class ResourceRefAttributeEdit extends AttributeInfoEdit {

    refListIndex: number;
    editField: Atomic.UIEditField;

    constructor(refListIndex: number = -1) {

        super();

        this.refListIndex = refListIndex;

    }

    onResourceChanged(resource: Atomic.Resource) {

        var parent = this.parent;

        while (parent) {

            if (parent.typeName == "UISection") {
                break;
            }

            parent = parent.parent;

        }

        if (parent) {

            parent.sendEvent(Editor.AttributeEditResourceChangedEventData({ attrInfoEdit: this, resource: resource }));

        }

    }

    initialize(editType: SerializableEditType, attrInfo: Atomic.AttributeInfo): boolean {

        if (!attrInfo.resourceTypeName)
            return false;

        if (this.refListIndex >= 0)
            this.nameOverride = attrInfo.resourceTypeName + " " + this.refListIndex;

        var importerName = ToolCore.assetDatabase.getResourceImporterName(attrInfo.resourceTypeName);

        if (!importerName)
            return false;

        return super.initialize(editType, attrInfo);
    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo, this.refListIndex);

        if (uniform) {

            var object = this.editType.getFirstObject();

            if (object) {

                // for cached resources, use the asset name, otherwise use the resource path name
                var resource: Atomic.Resource;
                if (this.refListIndex != -1) {
                    resource = object.getAttribute(this.attrInfo.name).resources[this.refListIndex];
                } else {
                    resource = <Atomic.Resource>object.getAttribute(this.attrInfo.name);
                }

                var text = "";

                if (resource) {
                    if (resource instanceof Atomic.Animation) {

                        text = (<Atomic.Animation>resource).animationName;

                    } else {

                        text = resource.name;
                        var asset = ToolCore.assetDatabase.getAssetByCachePath(resource.name);
                        if (asset)
                            text = asset.name;
                    }
                }
                this.editField.text = text;

                this.editField.subscribeToEvent(this.editField, Atomic.UIWidgetEvent((ev: Atomic.UIWidgetEvent) => {

                    if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_POINTER_DOWN) {

                        resource = <Atomic.Resource>object.getAttribute(this.attrInfo.name, this.arrayIndex);

                        if (resource instanceof Atomic.JSComponentFile) {

                            var pathName = resource.name;
                            this.sendEvent(Editor.InspectorProjectReferenceEventData({ "path": pathName }));

                        } else if (resource instanceof Atomic.Model) {

                            var asset = ToolCore.assetDatabase.getAssetByCachePath(resource.name);
                            this.sendEvent(Editor.InspectorProjectReferenceEventData({ "path": asset.getRelativePath() }));

                        } else if (resource instanceof Atomic.Animation) {

                             var animCacheReferenceName = resource.name.replace( "_" + (<Atomic.Animation>resource).animationName, "");
                             var asset = ToolCore.assetDatabase.getAssetByCachePath(animCacheReferenceName);
                             this.sendEvent(Editor.InspectorProjectReferenceEventData({ "path": asset.getRelativePath() }));

                        } else {

                            //Unknown Resource

                        }
                    }

                }));
            }


        } else {
            this.editField.text = "--";
        }

    }

    createEditWidget() {

        var layout = new Atomic.UILayout();
        var o = InspectorUtils.createAttrEditFieldWithSelectButton("", layout);
        this.editField = o.editField;

        layout.layoutSize = Atomic.UI_LAYOUT_SIZE.UI_LAYOUT_SIZE_AVAILABLE;
        layout.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_LEFT_RIGHT;
        layout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        o.editField.layoutParams = lp;
        o.editField.readOnly = true;

        this.editWidget = layout;

        var selectButton = o.selectButton;

        var resourceTypeName = this.attrInfo.resourceTypeName;
        var importerName = ToolCore.assetDatabase.getResourceImporterName(resourceTypeName);

        selectButton.onClick = () => {

            EditorUI.getModelOps().showResourceSelection("Select " + resourceTypeName + " Resource", importerName, resourceTypeName, function(retObject: any) {

                var resource: Atomic.Resource = null;

                if (retObject instanceof ToolCore.Asset) {

                    resource = (<ToolCore.Asset>retObject).getResource(resourceTypeName);

                } else if (retObject instanceof Atomic.Resource) {

                    resource = <Atomic.Resource>retObject;

                }

                this.editType.onAttributeInfoEdited(this.attrInfo, resource, this.refListIndex, true, this.arrayIndex);
                this.onResourceChanged(resource);
                this.refresh();

            }.bind(this));

        };

        // handle dropping of component on field
        this.editField.subscribeToEvent(this.editField, Atomic.DragEndedEvent((ev: Atomic.DragEndedEvent) => {

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

                    this.editType.onAttributeInfoEdited(this.attrInfo, resource, this.refListIndex, true, this.arrayIndex);
                    this.onResourceChanged(resource);
                    this.refresh();


                }
            }

        }));

    }

}

class ResourceRefListAttributeEdit extends AttributeInfoEdit {

    layout: Atomic.UILayout;
    refEdits: ResourceRefAttributeEdit[] = [];
    sizeEdit: Atomic.UIEditField;

    initialize(editType: SerializableEditType, attrInfo: Atomic.AttributeInfo): boolean {

        return super.initialize(editType, attrInfo);

    }

    createRefEdit(index: number) {

        var refEdit = new ResourceRefAttributeEdit(index);

        refEdit.initialize(this.editType, this.attrInfo);

        this.layout.addChild(refEdit);

        this.refEdits.push(refEdit);

    }

    createEditWidget() {

        this.spacing = 0;

        var layout = this.layout = new Atomic.UILayout();

        layout.axis = Atomic.UI_AXIS.UI_AXIS_Y;
        layout.spacing = 2;
        layout.layoutSize = Atomic.UI_LAYOUT_SIZE.UI_LAYOUT_SIZE_AVAILABLE;
        layout.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_LEFT_RIGHT;
        layout.layoutPosition = Atomic.UI_LAYOUT_POSITION.UI_LAYOUT_POSITION_LEFT_TOP;
        layout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var lp = new Atomic.UILayoutParams();
        lp.width = 304;
        layout.layoutParams = lp;

        var name = this.attrInfo.name + " Size";
        if (name == "AnimationResources Size")
            name = "Animations";

        var sizeEdit = this.sizeEdit = InspectorUtils.createAttrEditField(name, layout);

        lp = new Atomic.UILayoutParams();
        lp.width = 160;
        sizeEdit.layoutParams = lp;

        sizeEdit.subscribeToEvent(sizeEdit, Atomic.UIWidgetEditCompleteEvent((ev) => this.handleUIWidgetEditCompleteEvent(ev)));

        this.editWidget = layout;

    }

    createLayout() {

        this.createEditWidget();

        this.editWidget.subscribeToEvent(this.editWidget, Atomic.UIWidgetEvent((data) => this.handleWidgetEvent(data)));

        this.addChild(this.editWidget);

    }

    handleUIWidgetEditCompleteEvent(ev) {

        var size = Number(this.sizeEdit.text);

        if (size > 64 || size < 0)
            return;

        var editType = this.editType;

        var refresh = false;

        for (var i in editType.objects) {

            var object = editType.objects[i];
            var value = object.getAttribute(this.attrInfo.name);

            if (value.resources.length > size) {

                value.resources.length = size;
                object.setAttribute(this.attrInfo.name, value);
                refresh = true;

            } else if (value.resources.length < size) {

                for (var j = value.resources.length; j < size; j++) {

                    value.resources.push(null);

                }

                object.setAttribute(this.attrInfo.name, value);
                refresh = true;

            }

        }

        if (refresh)
            this.refresh();

    }

    refresh() {

        var editType = this.editType;

        var object = this.editType.getFirstObject();

        if (!object) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        this.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_VISIBLE;

        var maxLength = -1;
        var i;
        for (i in editType.objects) {

            object = editType.objects[i];
            var value = object.getAttribute(this.attrInfo.name);
            if (value.resources.length > maxLength) {

                maxLength = value.resources.length;

            }

        }

        this.sizeEdit.text = maxLength.toString();

        if (maxLength == -1) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        for (i = this.refEdits.length; i < maxLength; i++) {

            this.createRefEdit(i);

        }

        for (i = 0; i < this.refEdits.length; i++) {

            var refEdit = this.refEdits[i];

            if (i < maxLength) {
                refEdit.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_VISIBLE;
                refEdit.refresh();
            }
            else {
                refEdit.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
            }

        }

    }
}


class ArrayAttributeEdit extends AttributeInfoEdit {

    layout: Atomic.UILayout;
    indexEdits: AttributeInfoEdit[] = [];
    sizeEdit: Atomic.UIEditField;

    initialize(editType: SerializableEditType, attrInfo: Atomic.AttributeInfo): boolean {

        return super.initialize(editType, attrInfo);

    }

    createIndexEdit(index: number) {

        var indexEdit = AttributeInfoEdit.createAttrEdit(this.editType, this.attrInfo, index.toString(), this.attrInfo.type);
        indexEdit.arrayIndex = index;

        this.layout.addChild(indexEdit);

        this.indexEdits.push(indexEdit);

    }

    createEditWidget() {

        this.spacing = 0;

        var layout = this.layout = new Atomic.UILayout();

        layout.axis = Atomic.UI_AXIS.UI_AXIS_Y;
        layout.spacing = 2;
        layout.layoutSize = Atomic.UI_LAYOUT_SIZE.UI_LAYOUT_SIZE_AVAILABLE;
        layout.gravity = Atomic.UI_GRAVITY.UI_GRAVITY_LEFT_RIGHT;
        layout.layoutPosition = Atomic.UI_LAYOUT_POSITION.UI_LAYOUT_POSITION_LEFT_TOP;
        layout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var lp = new Atomic.UILayoutParams();
        lp.width = 304;
        layout.layoutParams = lp;

        var name = this.attrInfo.name;
        if (name == "AnimationResources")
            name = "Animations";

        var sizeEdit = this.sizeEdit = InspectorUtils.createAttrEditField(name, layout);

        if (this.attrInfo.fixedArraySize) {
            sizeEdit.disable();
        }

        lp = new Atomic.UILayoutParams();
        lp.width = 160;
        sizeEdit.layoutParams = lp;

        sizeEdit.subscribeToEvent(sizeEdit, Atomic.UIWidgetEditCompleteEvent((ev) => this.handleUIWidgetEditCompleteEvent(ev)));

        this.editWidget = layout;

    }

    createLayout() {

        this.createEditWidget();

        this.editWidget.subscribeToEvent(this.editWidget, Atomic.UIWidgetEvent((data) => this.handleWidgetEvent(data)));

        this.addChild(this.editWidget);

    }

    handleUIWidgetEditCompleteEvent(ev) {

        var size = Number(this.sizeEdit.text);

        if (size > 128 || size < 0)
            return;

        var editType = this.editType;

        var refresh = false;

        for (var i in editType.objects) {

            var object = editType.objects[i];

            var vector = <Atomic.ScriptVector>(object.getAttribute(this.attrInfo.name));

            if (vector.size != size) {

                vector.resize(size);
                object.setAttribute(this.attrInfo.name, vector);

                // record for undo/redo
                let scene = this.editType.getEditScene();
                if (scene) {
                    scene.sendEvent(Editor.SceneEditEndEventType);
                    scene.sendEvent(Editor.ComponentEditEndEventType);
                }

                refresh = true;
            }

        }

        if (refresh)
            this.refresh();

    }

    refresh() {

        var editType = this.editType;

        var object = this.editType.getFirstObject();

        if (!object) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        this.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_VISIBLE;

        var maxLength = -1;
        var i;
        for (i in editType.objects) {

            object = editType.objects[i];

            var vector = <Atomic.ScriptVector>(object.getAttribute(this.attrInfo.name));

            if (vector.size > maxLength) {

                maxLength = vector.size;
            }

        }

        this.sizeEdit.text = maxLength.toString();

        if (maxLength == -1) {
            this.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
            return;
        }

        for (i = this.indexEdits.length; i < maxLength; i++) {

            this.createIndexEdit(i);

        }

        for (i = 0; i < this.indexEdits.length; i++) {

            var indexEdit = this.indexEdits[i];

            if (i < maxLength) {
                indexEdit.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_VISIBLE;
                indexEdit.refresh();
            }
            else {
                indexEdit.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
            }

        }

    }

}


AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_BOOL] = BoolAttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_INT] = IntAttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_FLOAT] = FloatAttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_STRING] = StringAttributeEdit;

AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_VECTOR2] = Vector2AttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_VECTOR3] = Vector3AttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_QUATERNION] = QuaternionAttributeEdit;

AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_COLOR] = ColorAttributeEdit;

AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_RESOURCEREF] = ResourceRefAttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VariantType.VAR_RESOURCEREFLIST] = ResourceRefListAttributeEdit;

export = AttributeInfoEdit;
