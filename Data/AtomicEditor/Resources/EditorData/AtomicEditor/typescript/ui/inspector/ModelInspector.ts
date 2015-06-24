

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
        var modelSection = this.createSection("Model", 1);
        var modelVerticalLayout = this.createVerticalAttrLayout();
        rootLayout.addChild(modelSection);
        modelSection.contentRoot.addChild(modelVerticalLayout);

        this.scaleEdit = this.createAttrEditField("Scale", modelVerticalLayout);
        this.scaleEdit.text = this.importer.scale.toString();

        // Animations Section
        var animationSection = this.createSection("Animation", 1);
        var animVerticalLayout = this.createVerticalAttrLayout();
        rootLayout.addChild(animationSection);
        animationSection.contentRoot.addChild(animVerticalLayout);

        this.importAnimationBox = this.createAttrCheckBox("Import Animations", animVerticalLayout);
        this.importAnimationBox.value = this.importer.importAnimations ? 1 : 0;

        this.startTimeEdit = this.createAttrEditField("Start Time", animVerticalLayout);
        this.startTimeEdit.text = this.importer.startTime.toString();

        this.endTimeEdit = this.createAttrEditField("End Time", animVerticalLayout);
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
