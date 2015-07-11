
import ScriptWidget = require("../ScriptWidget");
import DataBinding = require("./DataBinding");
import InspectorUtils = require("./InspectorUtils");

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

        if (component.getTypeName() == "PrefabComponent") {

          this.addPrefabUI(attrsVerticalLayout);

        }

        if (component.getTypeName() == "Light") {
          this.addLightCascadeParametersUI(attrsVerticalLayout);
        }

        if (component.getTypeName() == "JSComponent") {
          this.addJSComponentUI(attrsVerticalLayout);
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

    addPrefabUI(layout:Atomic.UILayout) {

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

    addJSComponentUI(layout:Atomic.UILayout) {

      // expand prefab
      this.value = 1;

      var fd = new Atomic.UIFontDescription();
      fd.id = "Vera";
      fd.size = 11;

      var selectButton = new Atomic.UIButton();
      selectButton.text = "Select Script";
      selectButton.fontDescription = fd;

      selectButton.onClick = () => {

          return true;
      }

      var field = InspectorUtils.createAttrEditField("Script", layout);
      field.readOnly = true;

      layout.addChild(selectButton);

    }


    addLightCascadeParametersUI(layout:Atomic.UILayout) {

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

        return function(data:Atomic.UIWidgetEvent) {

          if (data.type == Atomic.UI_EVENT_TYPE_CHANGED) {

            this.light.setShadowCascadeParameter(this.index, Number(this.field.text));

          }

        }.bind({index:index, light:light, field:field});

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


    component: Atomic.Component;
    bindings: Array<DataBinding> = new Array();


}

export = ComponentInspector;
