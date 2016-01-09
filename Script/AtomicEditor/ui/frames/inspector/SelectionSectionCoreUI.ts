//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorUI = require("ui/EditorUI");
import InspectorUtils = require("./InspectorUtils");
import SelectionSection = require("./SelectionSection");
import SelectionSectionUI = require("./SelectionSectionUI");
import SerializableEditType = require("./SerializableEditType");

import ProgressModal = require("ui/modal/ProgressModal");


class CollisionShapeSectionUI extends SelectionSectionUI {

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

class CubemapGeneratorSectionUI extends SelectionSectionUI {

    createUI(editType: SerializableEditType) {

        this.editType = editType;

        var button = new Atomic.UIButton();
        button.fontDescription = InspectorUtils.attrFontDesc;
        button.gravity = Atomic.UI_GRAVITY_RIGHT;
        button.text = "Render Cubemap";

        button.onClick = () => {

            var scene = null;
            var count = 0;
            var progressModal = new ProgressModal("Rendering Cubemaps", "Rendering Cubemaps, please wait...");

            for (var i in this.editType.objects) {

                var gen = <Editor.CubemapGenerator>this.editType.objects[i];

                if (!scene) {

                    scene = gen.scene;

                    this.subscribeToEvent(scene, "CubemapRenderBegin", () => {

                        count++;

                    });

                    this.subscribeToEvent(scene, "CubemapRenderEnd", () => {

                        count--;

                        if (!count)
                            progressModal.hide();

                    });


                }

                if (!gen.render()) {

                    //TODO: Cancel other renders if any and report better error information

                    EditorUI.showModalError("Render Cubemaps",
                        "There was an error rendering cubemaps, please see the application log");

                    scene = null;
                    break;

                }
            }

            if (scene) {

                progressModal.show();

            }

        };

        this.addChild(button);

    }

}

SelectionSection.registerCustomSectionUI("CollisionShape", CollisionShapeSectionUI);
SelectionSection.registerCustomSectionUI("CubemapGenerator", CubemapGeneratorSectionUI);
