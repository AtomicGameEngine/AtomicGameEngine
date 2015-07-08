

import InspectorWidget = require("./InspectorWidget");
import ArrayEditWidget = require("./ArrayEditWidget");
import InspectorUtils = require("./InspectorUtils");

class ModelInspector extends InspectorWidget {

    constructor() {

        super();

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

    }

    onApply() {

        this.importer.scale = Number(this.scaleEdit.text);

        this.importer.importAnimations = this.importAnimationBox.value ? true : false;

        this.importer.import(this.asset.guid);
        this.asset.save();

    }

    inspect(asset: ToolCore.Asset) {

        this.asset = asset;
        this.importer = <ToolCore.ModelImporter> asset.importer;

        // node attr layout
        var rootLayout = this.rootLayout;

        // Model Section
        var modelLayout = this.createSection(rootLayout, "Model", 1);

        this.scaleEdit = this.createAttrEditField("Scale", modelLayout);
        this.scaleEdit.text = this.importer.scale.toString();

        // Animations Section
        var animationLayout = this.createSection(rootLayout, "Animation", 1);

        this.importAnimationBox = this.createAttrCheckBox("Import Animations", animationLayout);
        this.importAnimationBox.value = this.importer.importAnimations ? 1 : 0;

        this.importAnimationArray = new ArrayEditWidget("Animation Count");
        animationLayout.addChild(this.importAnimationArray);

        this.importAnimationArray.onCountChanged = (count) => this.onAnimationCountChanged(count);

        var nlp = new Atomic.UILayoutParams();
        nlp.width = 310;

        var animLayout = this.animationInfoLayout = new Atomic.UILayout();

        animLayout.spacing = 4;

        animLayout.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        animLayout.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        animLayout.layoutParams = nlp;
        animLayout.axis = Atomic.UI_AXIS_Y;
        animLayout.gravity = Atomic.UI_GRAVITY_ALL;

        animationLayout.addChild(animLayout);

        // apply button
        rootLayout.addChild(this.createApplyButton());

    }

    onAnimationCountChanged(count:number) {

      var layout = this.animationInfoLayout;
      layout.deleteAllChildren();

      for (var i = 0; i < count; i++) {

        var name = InspectorUtils.createAttrName("Animation " + i.toString() + ":");
        layout.addChild(name);

        InspectorUtils.createAttrEditField("Name", layout);
        InspectorUtils.createAttrEditField("Start", layout);
        InspectorUtils.createAttrEditField("End", layout);

        InspectorUtils.createSeparator(layout);

      }

    }

    // model
    scaleEdit: Atomic.UIEditField;

    // animation
    importAnimationBox: Atomic.UICheckBox;
    importAnimationArray: ArrayEditWidget;
    animationInfoLayout: Atomic.UILayout;

    asset: ToolCore.Asset;
    importer: ToolCore.ModelImporter;

}

export = ModelInspector;
