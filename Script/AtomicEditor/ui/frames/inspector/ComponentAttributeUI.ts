//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import InspectorUtils = require("./InspectorUtils");
import AttributeInfoEdit = require("./AttributeInfoEdit");

class LightCascadeAttributeEdit extends AttributeInfoEdit {

    splitFields: Atomic.UIEditField[] = [];

    createEditWidget() {

        var panel = new Atomic.UILayout();
        panel.axis = Atomic.UI_AXIS_Y;
        panel.layoutSize = Atomic.UI_LAYOUT_SIZE_PREFERRED;
        panel.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;

        var lp = new Atomic.UILayoutParams();
        lp.width = 180;
        panel.layoutParams = lp;

        this.editWidget = panel;

        var _this = this;

        function createHandler(index, field) {

            return function(ev: Atomic.UIWidgetEditCompleteEvent) {

                for (var i in _this.editType.objects) {

                    var o = <Atomic.Light>_this.editType.objects[i];
                    o.setShadowCascadeParameter(this.index, Number(this.field.text));

                }

                o.scene.sendEvent("SceneEditEnd");
                _this.refresh();

            }.bind({ index: index, field: field });

        }

        var flp = new Atomic.UILayoutParams();
        flp.width = 60;

        for (var i = 0; i < 4; i++) {
            var field = InspectorUtils.createAttrEditField("Split " + i, panel);
            field.layoutParams = flp;
            field.subscribeToEvent(field, "UIWidgetEditComplete", createHandler(i, field));
            this.splitFields.push(field);
        }

    }

    refresh() {

        // Vector 4 internally
        for (var i = 0; i < 4; i++) {

            var uniform = this.editType.getUniformValue(this.attrInfo, i);

            var field = this.splitFields[i];

            if (uniform) {

                var object = this.editType.getFirstObject();

                if (object) {
                    var value = object.getAttribute(this.attrInfo.name);
                    field.text = parseFloat(value[i].toFixed(5)).toString();
                }
                else {

                    field.text = "???";
                }
            }
            else {
                field.text = "--";
            }

        }


    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

            return true;
        }

        return false;

    }

}

AttributeInfoEdit.registerCustomAttr("Light", "CSM Splits", LightCascadeAttributeEdit);
