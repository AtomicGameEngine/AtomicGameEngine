
import SerializableEditType = require("./SerializableEditType");

class AttributeInfoEdit extends Atomic.UILayout {

    attrInfo: Atomic.AttributeInfo;
    editType: SerializableEditType;

    editWidget: Atomic.UIWidget;

    constructor() {

        super();

    }

    initialize(editType: SerializableEditType, attrInfo: Atomic.AttributeInfo) {

        this.editType = editType;
        this.attrInfo = attrInfo;

        this.createLayout();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        return false;

    }

    createLayout() {

        this.createEditWidget();

        this.editWidget.subscribeToEvent(this.editWidget, "WidgetEvent", (data) => this.handleWidgetEvent(data));

        var attr = this.attrInfo;
        var attrNameLP = AttributeInfoEdit.attrNameLP;

        this.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";
        name.layoutParams = attrNameLP;

        if (attr.type == Atomic.VAR_VECTOR3 || attr.type == Atomic.VAR_COLOR ||
            attr.type == Atomic.VAR_QUATERNION) {
            this.axis = Atomic.UI_AXIS_Y;
            this.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
            this.skinBg = "InspectorVectorAttrLayout";
        }

        var bname = attr.name;

        if (bname == "Is Enabled")
            bname = "Enabled";

        name.text = bname;
        name.fontDescription = AttributeInfoEdit.fontDesc;

        this.addChild(name);

        this.addChild(this.editWidget);

    }

    createEditWidget() {

    }

    refresh() {


    }

    static createAttrEdit(editType: SerializableEditType, attrInfo: Atomic.AttributeInfo): AttributeInfoEdit {

        var type: typeof AttributeInfoEdit;
        var customTypes = AttributeInfoEdit.customAttrEditTypes[editType.typeName];
        if (customTypes) {

            type = customTypes[attrInfo.name];

        }

        if (!type) {

            type = AttributeInfoEdit.standardAttrEditTypes[attrInfo.type];

        }

        if (!type)
            return null;

        var attrEdit = new type();
        attrEdit.initialize(editType, attrInfo);

        return attrEdit;

    }

    // atttribute name layout param
    static attrNameLP: Atomic.UILayoutParams;
    static fontDesc: Atomic.UIFontDescription;

    static standardAttrEditTypes: { [variantType: number /*Atomic.VariantType*/]: typeof AttributeInfoEdit } = {};

    static customAttrEditTypes: { [typeName: string]: { [name: string]: typeof AttributeInfoEdit } } = {};

    private static Ctor = (() => {

        var attrNameLP = AttributeInfoEdit.attrNameLP = new Atomic.UILayoutParams();
        attrNameLP.width = 100;

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
            this.editWidget.skinBg = "TBGreyCheckBox";
            if (object) {
                var value = object.getAttribute(this.attrInfo.name);
                this.editWidget.value = (value ? 1 : 0);
            }

        } else {

            this.editWidget.skinBg = "TBGreyCheckBoxNonUniform";
            this.editWidget.value = 1;

        }

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

            this.editType.onAttributeInfoEdited(this.attrInfo, this.editWidget.value ? true : false);
            this.refresh();

            return true;
        }

        return false;

    }

}

class StringAttributeEdit extends AttributeInfoEdit {

    createEditWidget() {

        var field = new Atomic.UIEditField();
        field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        field.skinBg = "TBAttrEditorField";;
        field.fontDescription = AttributeInfoEdit.fontDesc;
        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        field.layoutParams = lp;

        field.subscribeToEvent(field, "UIWidgetEditComplete", (ev) => this.handleUIWidgetEditCompleteEvent(ev));

        this.editWidget = field;
    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo);

        if (uniform) {
            var object = this.editType.getFirstObject();
            if (object) {
                var value = object.getAttribute(this.attrInfo.name);
                this.editWidget.text = value;
            }

        } else {

            this.editWidget.text = "--";

        }

    }

    handleUIWidgetEditCompleteEvent(ev) {

        this.editType.onAttributeInfoEdited(this.attrInfo, this.editWidget.text);
        this.refresh();

    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

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
            field.textAlign = Atomic.UI_TEXT_ALIGN_CENTER;
            field.skinBg = "TBAttrEditorField";;
            field.fontDescription = AttributeInfoEdit.fontDesc;
            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            field.layoutParams = lp;

            field.subscribeToEvent(field, "UIWidgetEditComplete", (ev) => this.handleUIWidgetEditCompleteEvent(ev));

            this.editWidget = field;
        }
    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo);

        if (uniform) {
            var object = this.editType.getFirstObject();
            if (object) {
                var value = object.getAttribute(this.attrInfo.name);

                var widget = this.editWidget;
                var attrInfo = this.attrInfo;

                if (attrInfo.enumNames.length) {
                    widget.text = attrInfo.enumNames[value];
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
        this.editType.onAttributeInfoEdited(this.attrInfo, Number(this.editWidget.text));
        this.refresh();

    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

            return true;
        }

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = this.attrInfo.name + " enum popup";

            if (ev.target.id == id) {

                this.editType.onAttributeInfoEdited(this.attrInfo, Number(ev.refid) - 1);
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
        field.textAlign = Atomic.UI_TEXT_ALIGN_CENTER;
        field.skinBg = "TBAttrEditorField";;
        field.fontDescription = AttributeInfoEdit.fontDesc;
        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        field.layoutParams = lp;

        field.subscribeToEvent(field, "UIWidgetEditComplete", (ev) => this.handleUIWidgetEditCompleteEvent(ev));

        this.editWidget = field;

    }

    refresh() {

        var uniform = this.editType.getUniformValue(this.attrInfo);

        if (uniform) {
            var object = this.editType.getFirstObject();
            if (object) {

                var value = object.getAttribute(this.attrInfo.name);

                var widget = this.editWidget;
                var attrInfo = this.attrInfo;

                var value = object.getAttribute(attrInfo.name);
                widget.text = parseFloat(value.toFixed(5)).toString();

            }

        } else {

            this.editWidget.text = "--";

        }

    }

    handleUIWidgetEditCompleteEvent(ev) {

        this.editType.onAttributeInfoEdited(this.attrInfo, Number(this.editWidget.text));
        this.refresh();

    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

            return true;
        }

        return false;

    }

}

class Vector3AttributeEdit extends AttributeInfoEdit {

    selects: Atomic.UIInlineSelect[] = [];

    createEditWidget() {

        var attrInfo = this.attrInfo;

        var layout = new Atomic.UILayout();
        layout.spacing = 0;

        var lp = new Atomic.UILayoutParams();
        lp.width = 100;

        for (var i = 0; i < 3; i++) {

            var select = new Atomic.UIInlineSelect();
            this.selects.push(select);

            select.id = String(i + 1);
            select.fontDescription = AttributeInfoEdit.fontDesc;
            select.skinBg = "InspectorVectorAttrName";
            select.setLimits(-10000000, 10000000);
            var editlp = new Atomic.UILayoutParams();
            editlp.minWidth = 60;
            select.editFieldLayoutParams = editlp;
            select.layoutParams = lp;
            layout.addChild(select);

            select.subscribeToEvent(select, "WidgetEvent", (ev) => this.handleWidgetEvent(ev));
            select.subscribeToEvent(select, "UIWidgetEditComplete", (ev) => this.handleUIWidgetEditCompleteEvent(ev));
        }

        this.editWidget = layout;

    }

    refresh() {

        for (var i in this.selects) {

            var select = this.selects[i];
            var edit = select.getWidget("edit");

            var uniform = this.editType.getUniformValue(this.attrInfo, i);

            if (uniform) {

                var object = this.editType.getFirstObject();

                if (object) {

                    var value = object.getAttribute(this.attrInfo.name);
                    select.value = parseFloat(value[i].toFixed(5));

                }

            } else {

                edit.text = "--";

            }

        }


    }

    handleUIWidgetEditCompleteEvent(ev: Atomic.UIWidgetEditCompleteEvent) {

        var index = Number(ev.widget.id) - 1;
        this.editType.onAttributeInfoEdited(this.attrInfo, ev.widget.value, index);
        this.refresh();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

            var index = Number(ev.target.id) - 1;
            this.editType.onAttributeInfoEdited(this.attrInfo, ev.target.value, index, false);

            return true;
        }

        return false;

    }

}

AttributeInfoEdit.standardAttrEditTypes[Atomic.VAR_BOOL] = BoolAttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VAR_INT] = IntAttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VAR_FLOAT] = FloatAttributeEdit;
AttributeInfoEdit.standardAttrEditTypes[Atomic.VAR_STRING] = StringAttributeEdit;

AttributeInfoEdit.standardAttrEditTypes[Atomic.VAR_VECTOR3] = Vector3AttributeEdit;

export = AttributeInfoEdit;
