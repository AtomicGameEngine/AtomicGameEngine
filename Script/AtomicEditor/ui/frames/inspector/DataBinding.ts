import InspectorUtils = require("./InspectorUtils");
import EditorUI = require("ui/EditorUI");

class DataBinding {

    constructor(object: Atomic.Serializable, attrInfo: Atomic.AttributeInfo, widget: Atomic.UIWidget) {

        this.object = object;
        this.attrInfo = attrInfo;
        this.widget = widget;

    }

    static createBinding(object: Atomic.Serializable, attrInfo: Atomic.AttributeInfo): DataBinding {

        var widget: Atomic.UIWidget = null;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var enumSource = null;

        if (attrInfo.type == Atomic.VAR_BOOL) {

            var box = new Atomic.UICheckBox();
            box.skinBg = "TBGreyCheckBox";
            widget = box;
        }
        else if (attrInfo.type == Atomic.VAR_INT) {

            if (attrInfo.enumNames.length) {

                enumSource = new Atomic.UISelectItemSource();

                for (var i in attrInfo.enumNames) {

                    enumSource.addItem(new Atomic.UISelectItem(attrInfo.enumNames[i], (Number(i) + 1).toString()));

                }

                var button = new Atomic.UIButton();
                button.fontDescription = fd;
                button.text = "Enum Value!";
                var lp = new Atomic.UILayoutParams();
                lp.width = 140;
                button.layoutParams = lp;

                widget = button;

            } else {

                var field = new Atomic.UIEditField();
                field.textAlign = Atomic.UI_TEXT_ALIGN_CENTER;
                field.fontDescription = fd;
                field.skinBg = "TBAttrEditorField";

                var lp = new Atomic.UILayoutParams();
                lp.width = 140;
                field.layoutParams = lp;

                widget = field;
            }

        } else if (attrInfo.type == Atomic.VAR_FLOAT) {

            var field = new Atomic.UIEditField();
            field.textAlign = Atomic.UI_TEXT_ALIGN_CENTER;
            field.fontDescription = fd;
            field.skinBg = "TBAttrEditorField";

            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            field.layoutParams = lp;

            widget = field;

        }
        else if (attrInfo.type == Atomic.VAR_STRING) {

            var field = new Atomic.UIEditField();
            field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            field.skinBg = "TBAttrEditorField";;
            field.fontDescription = fd;
            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            field.layoutParams = lp;

            widget = field;
        }
        else if (attrInfo.type == Atomic.VAR_VECTOR3 || attrInfo.type == Atomic.VAR_QUATERNION) {
            var layout = new Atomic.UILayout();
            widget = layout;
            layout.spacing = 0;

            var lp = new Atomic.UILayoutParams();
            lp.width = 100;

            for (var i: any = 0; i < 3; i++) {
                var select = new Atomic.UIInlineSelect();
                select.id = String(i + 1);
                select.fontDescription = fd;
                select.skinBg = "InspectorVectorAttrName";
                select.setLimits(-10000000, 10000000);
                var editlp = new Atomic.UILayoutParams();
                editlp.minWidth = 60;
                select.editFieldLayoutParams = editlp;
                select.layoutParams = lp;
                layout.addChild(select);
            }

        } else if (attrInfo.type == Atomic.VAR_COLOR) {
            var layout = new Atomic.UILayout();
            widget = layout;
            layout.spacing = 0;

            var lp = new Atomic.UILayoutParams();
            lp.width = 70;

            for (var i: any = 0; i < 4; i++) {

                var select = new Atomic.UIInlineSelect();
                select.id = String(i + 1);
                select.fontDescription = fd;
                select.setLimits(-10000000, 10000000);
                select.layoutParams = lp;
                layout.addChild(select);
            }

        } else if (attrInfo.type == Atomic.VAR_VECTOR2) {
            var layout = new Atomic.UILayout();
            widget = layout;
            layout.spacing = 0;

            var lp = new Atomic.UILayoutParams();
            lp.width = 100;

            for (var i: any = 0; i < 2; i++) {
                var select = new Atomic.UIInlineSelect();
                select.id = String(i + 1);
                select.fontDescription = fd;
                select.skinBg = "InspectorVectorAttrName";
                select.setLimits(-10000000, 10000000);
                var editlp = new Atomic.UILayoutParams();
                editlp.minWidth = 60;
                select.editFieldLayoutParams = editlp;
                select.layoutParams = lp;
                layout.addChild(select);
            }

        } else if (attrInfo.type == Atomic.VAR_RESOURCEREF && attrInfo.resourceTypeName) {

            var importerName = ToolCore.assetDatabase.getResourceImporterName(attrInfo.resourceTypeName);

            if (importerName) {

                var parent = new Atomic.UILayout();
                var o = InspectorUtils.createAttrEditFieldWithSelectButton("", parent);

                parent.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;
                parent.gravity = Atomic.UI_GRAVITY_LEFT_RIGHT;
                parent.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;


                var lp = new Atomic.UILayoutParams();
                lp.width = 140;
                o.editField.layoutParams = lp;
                o.editField.readOnly = true;

                // stuff editfield in so can be reference
                parent["editField"] = o.editField;

                var selectButton = o.selectButton;

                selectButton.onClick = () => {

                    EditorUI.getModelOps().showResourceSelection("Select " + attrInfo.resourceTypeName + " Resource", importerName, function(asset: ToolCore.Asset) {

                        var resource = asset.getResource(attrInfo.resourceTypeName);

                        object.setAttribute(attrInfo.name, resource);

                        if (resource) {

                            // use the asset name instead of the cache name
                            if (asset.importer.requiresCacheFile())
                                o.editField.text = asset.name;
                            else
                                o.editField.text = resource.name;
                        }
                        else
                            o.editField.text = "";


                    });

                }

                // handle dropping of component on field
                o.editField.subscribeToEvent(o.editField, "DragEnded", (ev: Atomic.DragEndedEvent) => {

                    if (ev.target == o.editField) {

                        var dragObject = ev.dragObject;

                        var importer;

                        if (dragObject.object && dragObject.object.typeName == "Asset") {

                            var asset = <ToolCore.Asset> dragObject.object;

                            if (asset.importerTypeName == importerName) {
                                importer = asset.importer;
                            }

                        }

                        if (importer) {

                            var resource = asset.getResource(attrInfo.resourceTypeName);
                            object.setAttribute(attrInfo.name, resource);
                            if (resource) {
                                // use the asset name instead of the cache name
                                if (asset.importer.requiresCacheFile())
                                    o.editField.text = asset.name;
                                else
                                    o.editField.text = resource.name;
                            }
                            else
                                o.editField.text = "";

                        }
                    }

                });

                widget = parent;
            }

        }

        if (widget) {

            var binding = new DataBinding(object, attrInfo, widget);
            binding.enumSource = enumSource;
            return binding;

        }

        return null;

    }

    setWidgetValueFromObject() {
        if (this.widgetLocked)
            return;

        this.widgetLocked = true;

        var attrInfo = this.attrInfo;
        var object = this.object;
        var widget = this.widget;

        if (attrInfo.type == Atomic.VAR_BOOL) {
            var value = object.getAttribute(attrInfo.name);
            widget.value = (value ? 1 : 0);
        }
        else if (attrInfo.type == Atomic.VAR_VECTOR3) {

            var value = object.getAttribute(attrInfo.name);

            for (var i = 0; i < 3; i++) {

                var select = widget.getWidget((i + 1).toString());
                if (select)
                    select.value = value[i];
            }

        }
        else if (attrInfo.type == Atomic.VAR_VECTOR2) {

            var value = object.getAttribute(attrInfo.name);

            for (var i = 0; i < 2; i++) {

                var select = widget.getWidget((i + 1).toString());
                if (select)
                    select.value = value[i];
            }

        }
        else if (attrInfo.type == Atomic.VAR_QUATERNION) {

            var value = object.getAttribute(attrInfo.name);

            for (var i = 0; i < 3; i++) {

                var select = widget.getWidget((i + 1).toString());
                if (select)
                    select.value = value[i];
            }

        }
        else if (attrInfo.type == Atomic.VAR_STRING) {
            var value = object.getAttribute(attrInfo.name);
            widget.text = value;
        }
        else if (attrInfo.type == Atomic.VAR_FLOAT) {
            var value = object.getAttribute(attrInfo.name);
            widget.text = parseFloat(value.toFixed(5)).toString();
        }
        else if (attrInfo.type == Atomic.VAR_INT) {
            var value = object.getAttribute(attrInfo.name);

            if (attrInfo.enumNames.length) {
                widget.text = attrInfo.enumNames[value];
            }
            else {
                widget.text = value.toString();
            }
        }
        else if (attrInfo.type == Atomic.VAR_COLOR) {

            var value = object.getAttribute(attrInfo.name);

            for (var i = 0; i < 4; i++) {

                var select = widget.getWidget((i + 1).toString());
                if (select)
                    select.value = value[i];
            }

        } else if (attrInfo.type == Atomic.VAR_RESOURCEREF && attrInfo.resourceTypeName) {

            // for cached resources, use the asset name, otherwise use the resource path name
            var resource = <Atomic.Resource> object.getAttribute(attrInfo.name);
            var text = "";
            if (resource) {
                text = resource.name;
                var asset = ToolCore.assetDatabase.getAssetByCachePath(resource.name);
                if (asset)
                  text = asset.name;
            }

            widget["editField"].text = text;

        }

        this.widgetLocked = false;

    }

    setObjectValueFromWidget(srcWidget: Atomic.UIWidget) {

        if (this.objectLocked)
            return;

        this.objectLocked = true;

        var widget = this.widget;
        var object = this.object;
        var attrInfo = this.attrInfo;
        var type = attrInfo.type;

        if (type == Atomic.VAR_BOOL) {

            object.setAttribute(attrInfo.name, widget.value ? true : false);

        } else if (type == Atomic.VAR_INT) {

            object.setAttribute(attrInfo.name, Number(widget.text));

        }
        else if (type == Atomic.VAR_FLOAT) {

            object.setAttribute(attrInfo.name, Number(widget.text));

        }
        else if (type == Atomic.VAR_STRING) {

            object.setAttribute(attrInfo.name, widget.text);

        } else if (type == Atomic.VAR_VECTOR3 && srcWidget) {

            var value = object.getAttribute(attrInfo.name);

            if (srcWidget.id == "1")
                value[0] = srcWidget.value;
            else if (srcWidget.id == "2")
                value[1] = srcWidget.value;
            else if (srcWidget.id == "3")
                value[2] = srcWidget.value;

            object.setAttribute(attrInfo.name, value);

        } else if (type == Atomic.VAR_VECTOR2 && srcWidget) {

            var value = object.getAttribute(attrInfo.name);

            if (srcWidget.id == "1")
                value[0] = srcWidget.value;
            else if (srcWidget.id == "2")
                value[1] = srcWidget.value;

            object.setAttribute(attrInfo.name, value);

        }
        else if (type == Atomic.VAR_QUATERNION && srcWidget) {

            var value = object.getAttribute(attrInfo.name);

            if (srcWidget.id == "1")
                value[0] = srcWidget.value;
            else if (srcWidget.id == "2")
                value[1] = srcWidget.value;
            else if (srcWidget.id == "3")
                value[2] = srcWidget.value;

            object.setAttribute(attrInfo.name, value);

        } else if (type == Atomic.VAR_COLOR && srcWidget) {

            var value = object.getAttribute(attrInfo.name);

            if (srcWidget.id == "1")
                value[0] = srcWidget.value;
            else if (srcWidget.id == "2")
                value[1] = srcWidget.value;
            else if (srcWidget.id == "3")
                value[2] = srcWidget.value;
            else if (srcWidget.id == "4")
                value[3] = srcWidget.value;

            object.setAttribute(attrInfo.name, value);
        }

        this.objectLocked = false;

    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (this.objectLocked)
            return false;

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = this.attrInfo.name + " enum popup";

            if (ev.target.id == id) {

                this.object.setAttribute(this.attrInfo.name, Number(ev.refid) - 1);
                this.setWidgetValueFromObject();

            }

            else if (this.widget == ev.target && this.attrInfo.enumNames.length) {


                if (this.enumSource) {
                    var menu = new Atomic.UIMenuWindow(ev.target, id);
                    menu.show(this.enumSource);
                }

                return true;

            }


        }

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {
            if (this.widget == ev.target || this.widget.isAncestorOf(ev.target)) {
                this.setObjectValueFromWidget(ev.target);
                return true;
            }
        }

        return false;

    }

    object: Atomic.Serializable;
    objectLocked: boolean = true;
    widgetLocked: boolean = false;
    attrInfo: Atomic.AttributeInfo;
    widget: Atomic.UIWidget;
    enumSource: Atomic.UISelectItemSource;

}

export = DataBinding;
