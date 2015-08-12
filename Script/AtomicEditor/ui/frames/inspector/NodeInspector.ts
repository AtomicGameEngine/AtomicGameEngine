
import ScriptWidget = require("ui/ScriptWidget");
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

    getPrefabComponent(node: Atomic.Node): Atomic.PrefabComponent {

        if (node.getComponent("PrefabComponent"))
            return <Atomic.PrefabComponent> node.getComponent("PrefabComponent");

        if (node.parent)
            return this.getPrefabComponent(node.parent);

        return null;

    }

    detectPrefab(node: Atomic.Node): boolean {

        if (node.getComponent("PrefabComponent"))
            return true;

        if (node.parent)
            return this.detectPrefab(node.parent);

        return false;

    }


    inspect(node: Atomic.Node) {

        this.bindings = new Array();

        this.node = node;

        this.isPrefab = this.detectPrefab(node);

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var nlp = new Atomic.UILayoutParams();
        nlp.width = 304;

        var nodeLayout = this.nodeLayout = new Atomic.UILayout();
        nodeLayout.spacing = 4;

        nodeLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        nodeLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        nodeLayout.layoutParams = nlp;
        nodeLayout.axis = Atomic.UI_AXIS_Y;

        // node attr layout

        var nodeSection = new Atomic.UISection();
        nodeSection.text = "Node";
        nodeSection.value = 1;
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

        // PREFAB

        if (this.isPrefab) {

            var name = new Atomic.UITextField();
            name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
            name.skinBg = "InspectorTextAttrName";
            name.text = "Prefab"
            name.fontDescription = fd;

            var prefabLayout = new Atomic.UILayout();
            prefabLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

            var saveButton = new Atomic.UIButton();
            saveButton.text = "Save";
            saveButton.fontDescription = fd;

            saveButton.onClick = () => {

                var prefabComponent = this.getPrefabComponent(this.node);

                if (prefabComponent) {

                    prefabComponent.savePrefab();

                    this.sendEvent("EditorActiveNodeChange", { node: this.node });

                    return true;

                }

            }

            var undoButton = new Atomic.UIButton();
            undoButton.text = "Undo";
            undoButton.fontDescription = fd;

            undoButton.onClick = () => {

                var prefabComponent = this.getPrefabComponent(this.node);

                if (prefabComponent) {

                    prefabComponent.undoPrefab();

                    this.sendEvent("EditorActiveNodeChange", { node: this.node });

                    return true;

                }

            }

            var breakButton = new Atomic.UIButton();
            breakButton.text = "Break";
            breakButton.fontDescription = fd;

            breakButton.onClick = () => {

                var prefabComponent = this.getPrefabComponent(this.node);

                if (prefabComponent) {

                    prefabComponent.breakPrefab();

                    return true;

                }

            }


            prefabLayout.addChild(name);
            prefabLayout.addChild(saveButton);
            prefabLayout.addChild(undoButton);
            prefabLayout.addChild(breakButton);

            attrsVerticalLayout.addChild(prefabLayout);

        }

        // COMPONENTS

        var components = node.getComponents();

        for (var i in components) {

            var component = components[i];

            //if (component.isTemporary())
            //  continue;

            var ci = new ComponentInspector();

            ci.inspect(component);

            nodeLayout.addChild(ci);

        }

        this.addChild(nodeLayout);

        var button = new CreateComponentButton(node);

        nodeLayout.addChild(button);

        for (var i in this.bindings) {
            this.bindings[i].setWidgetValueFromObject();
            this.bindings[i].objectLocked = false;
        }

    }

    isPrefab: boolean;
    node: Atomic.Node;
    nodeLayout: Atomic.UILayout;
    bindings: Array<DataBinding>;


}

export = NodeInspector;
