

import ScriptWidget = require("../ScriptWidget");

class ModelInspector extends ScriptWidget {

    constructor() {

        super();

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

    }

    inspect(asset: ToolCore.Asset) {

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        var nlp = new Atomic.UILayoutParams();
        nlp.width = 320;

        var assetLayout = new Atomic.UILayout();
        assetLayout.spacing = 4;

        assetLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        assetLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        assetLayout.layoutParams = nlp;
        assetLayout.axis = Atomic.UI_AXIS_Y;

        // node attr layout

        var modelSection = new Atomic.UISection();
        modelSection.text = "Model Import";
        modelSection.value = 1;
        //modelSection.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        //modelSection.skinBg = "InspectorTextLabel";
        assetLayout.addChild(modelSection);

        var attrsVerticalLayout = new Atomic.UILayout(Atomic.UI_AXIS_Y);
        attrsVerticalLayout.spacing = 3;
        attrsVerticalLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        attrsVerticalLayout.layoutSize = Atomic.UI_LAYOUT_SIZE_AVAILABLE;

        modelSection.contentRoot.addChild(attrsVerticalLayout);

        var attrLayout = new Atomic.UILayout();

        attrLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;

        var name = new Atomic.UITextField();
        name.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        name.skinBg = "InspectorTextAttrName";
        name.text = "Scale";
        name.fontDescription = fd;
        attrLayout.addChild(name);

        var scaleEdit = new Atomic.UIEditField();
        scaleEdit.textAlign = Atomic.UI_TEXT_ALIGN_LEFT;
        scaleEdit.skinBg = "TBAttrEditorField";;
        scaleEdit.fontDescription = fd;
        var lp = new Atomic.UILayoutParams();
        lp.width = 180;
        scaleEdit.layoutParams = lp;

        var importer = <ToolCore.ModelImporter> asset.importer;

        scaleEdit.text = importer.scale.toString();

        attrLayout.addChild(scaleEdit);

        attrsVerticalLayout.addChild(attrLayout);

        this.addChild(assetLayout);

    }

    bindings:Array<DataBinding> = new Array();


}

export = ModelInspector;
