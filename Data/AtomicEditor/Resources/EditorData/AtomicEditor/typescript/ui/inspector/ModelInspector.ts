

import InspectorWidget = require("./InspectorWidget");

class ModelInspector extends InspectorWidget {

    constructor() {

        super();

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

    }

    onApply() {

      this.importer.scale = Number(this.scaleEdit.text);
      this.importer.startTime = Number(this.startTimeEdit.text);
      this.importer.endTime = Number(this.endTimeEdit.text);

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

        this.startTimeEdit = this.createAttrEditField("Start Time", animationLayout);
        this.startTimeEdit.text = this.importer.startTime.toString();

        this.endTimeEdit = this.createAttrEditField("End Time", animationLayout);
        this.endTimeEdit.text = this.importer.endTime.toString();

        // apply button
        rootLayout.addChild(this.createApplyButton());

    }

    // model
    scaleEdit:Atomic.UIEditField;

    // animation
    startTimeEdit:Atomic.UIEditField;
    endTimeEdit:Atomic.UIEditField;
    importAnimationBox:Atomic.UICheckBox;

    asset:ToolCore.Asset;
    importer:ToolCore.ModelImporter;

}

export = ModelInspector;
