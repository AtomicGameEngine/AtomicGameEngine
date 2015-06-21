
import ScriptWidget = require("../ScriptWidget");

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

        if (attrInfo.type == Atomic.VAR_BOOL) {

            var box = new Atomic.UICheckBox();
            box.skinBg = "TBGreyCheckBox";
            widget = box;

        } else if (attrInfo.type == Atomic.VAR_STRING) {

            var field = new Atomic.UIEditField();
            field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            field.skinBg = "TBAttrEditorField";;
            field.fontDescription = fd;
            var lp = new Atomic.UILayoutParams();
            lp.width = 140;
            field.layoutParams = lp;

            widget = field;
        }
        else if (attrInfo.type == Atomic.VAR_VECTOR3 || attrInfo.type == Atomic.VAR_QUATERNION)
        {
            var layout = new Atomic.UILayout();
            widget = layout;
            layout.spacing = 0;

            var lp = new Atomic.UILayoutParams();
            lp.width = 100;

            for (var i = 0; i < 3; i++)
            {
                var select = new Atomic.UIInlineSelect();
                select.id = String(i + 1);
                select.fontDescription = fd;
                select.skinBg = "InspectorTextAttrName";
                select.setLimits(-10000000, 10000000);
                var editlp = new Atomic.UILayoutParams();
                editlp.minWidth = 60;
                select.editFieldLayoutParams = editlp;
                select.layoutParams = lp;
                layout.addChild(select);
            }

        }



        if (widget) {

            var binding = new DataBinding(object, attrInfo, widget);
            return binding;

        }

        return null;

    }

    object: Atomic.Serializable;
    attrInfo: Atomic.AttributeInfo;
    widget: Atomic.UIWidget;

}

class NodeInspector extends ScriptWidget {

    constructor() {

        super();

    }

    inspect(node: Atomic.Node) {

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var nlp = new Atomic.UILayoutParams();
        nlp.width = 304;

        var nodeLayout = new Atomic.UILayout();
        nodeLayout.spacing = 4;

        nodeLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        nodeLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        nodeLayout.layoutParams = nlp;

        // node attr layout

        var nodeSection = new Atomic.UISection();
        nodeSection.text = "Node";
        nodeSection.value = 1;
        //nodeSection.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        //nodeSection.skinBg = "InspectorTextLabel";
        nodeLayout.addChild(nodeSection);

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        nodeSection.contentRoot.addChild(attrsVerticalLayout);

        var attrs = node.getAttributes();

        for (var i in attrs) {

            var attr = <Atomic.AttributeInfo> attrs[i];

            if (attr.mode & Atomic.AM_NOEDIT)
                continue;

            var binding = DataBinding.createBinding(node, attr);

            if (!binding)
                continue;

            var attrLayout = new Atomic.UILayout();

            attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

            var name = new Atomic.UITextField();
            name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            name.skinBg = "InspectorTextAttrName";

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

        }

        this.addChild(nodeLayout);

    }


}

export = NodeInspector;
