
import ScriptWidget = require("../ScriptWidget");

class MaterialInspector extends ScriptWidget {

    constructor() {

        super();

    }

    inspect(material: Atomic.Material) {

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var mlp = new Atomic.UILayoutParams();
        mlp.width = 304;

        var materialLayout = new Atomic.UILayout();
        materialLayout.spacing = 4;

        materialLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        materialLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        materialLayout.layoutParams = mlp;
        materialLayout.axis = Atomic.UI_AXIS_Y;

        // node attr layout

        var materialSection = new Atomic.UISection();
        materialSection.text = "Material";
        materialSection.value = 1;
        materialLayout.addChild(materialSection);

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        var attrLayout = new Atomic.UILayout();
        attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";

        name.text = "Name";
        name.fontDescription = fd;

        attrLayout.addChild(name);


        /*
        var field = new Atomic.UIEditField();
        field.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        field.skinBg = "TBAttrEditorField";;
        field.fontDescription = fd;
        var lp = new Atomic.UILayoutParams();
        lp.width = 140;
        field.layoutParams = lp;

        field.text = material.getTexture(Atomic.TU_DIFFUSE).name;

        attrLayout.addChild(field);

        */



        var textureWidget = new Atomic.UITextureWidget();
        textureWidget.texture = material.getTexture(Atomic.TU_DIFFUSE);

        var tlp = new Atomic.UILayoutParams();
        tlp.width = 64;
        tlp.height = 64;
        textureWidget.layoutParams = tlp;


        attrLayout.addChild(textureWidget);


        attrsVerticalLayout.addChild(attrLayout);


        materialSection.contentRoot.addChild(attrsVerticalLayout);


        /*

        // TODO: support quality level and lod for techniques
        var technique = material.getTechnique(0);

        this.nameTextField.text = technique.name;

        var params = material.getShaderParameters();

        for (var i in params) {

          print(params[i].name, " : ", params[i].value, " : ", params[i].type);

        }
        */

        this.addChild(materialLayout);

    }

    material: Atomic.Material;

    nameTextField: Atomic.UITextField;


}

export = MaterialInspector;
