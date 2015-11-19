

import InspectorUtils = require("./InspectorUtils");
import SelectionSection = require("./SelectionSection");
import SelectionSectionUI = require("./SelectionSectionUI");
import SerializableEditType = require("./SerializableEditType");


class CollisionShapeSectionUI extends SelectionSectionUI {

    editType: SerializableEditType;

    createUI(editType: SerializableEditType) {

        this.editType = editType;

        var button = new Atomic.UIButton();
        button.fontDescription = InspectorUtils.attrFontDesc;
        button.gravity = Atomic.UI_GRAVITY_RIGHT;
        button.text = "Set from StaticModel";

        button.onClick = () => {

            for (var i in this.editType.objects) {

                var shape = <Atomic.CollisionShape>this.editType.objects[i];
                var model = <Atomic.Drawable>shape.node.getComponent("StaticModel");

                if (model) {
                    var box = model.boundingBox;
                    shape.setBox([box[3] - box[0], box[4] - box[1], box[5] - box[2]]);
                }

            }

        };

        this.addChild(button);

    }

}

SelectionSection.registerCustomSectionUI("CollisionShape", CollisionShapeSectionUI);
