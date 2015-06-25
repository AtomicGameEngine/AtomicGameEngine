
import ScriptWidget = require("../ScriptWidget");
import ComponentInspector = require("./ComponentInspector");
import DataBinding = require("./DataBinding");
import CreateComponentButton = require("./CreateComponentButton");

class NodeInspector extends ScriptWidget {

    constructor() {

        super();

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        var handled = false;

        for (var i = 0; i < this.bindings.length; i++) {

          if (this.bindings[i].handleWidgetEvent(ev)) {

            handled = true;

          }

        }

        // return handled
        return handled;

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
        nodeLayout.axis = Atomic.UI_AXIS_Y;

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

            this.bindings.push(binding);

        }

        var components = node.getComponents();

        for (var i in components) {

          var ci = new ComponentInspector();

          ci.inspect(components[i]);

          nodeLayout.addChild(ci);

        }

        this.addChild(nodeLayout);

        var button = new CreateComponentButton(node);

        nodeLayout.addChild(button);

    }

    bindings:Array<DataBinding> = new Array();


}

export = NodeInspector;
