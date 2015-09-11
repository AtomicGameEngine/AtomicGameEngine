//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import InspectorUtils = require("./InspectorUtils");

class ArrayEditWidget extends Atomic.UILayout {

    constructor(title:string) {

        super();

        var nlp = new Atomic.UILayoutParams();
        nlp.width = 310;

        this.spacing = 4;

        this.layoutDistribution = Atomic.UI_LAYOUT_DISTRIBUTION_GRAVITY;
        this.layoutPosition = Atomic.UI_LAYOUT_POSITION_LEFT_TOP;
        this.layoutParams = nlp;
        this.axis = Atomic.UI_AXIS_Y;

        this.gravity = Atomic.UI_GRAVITY_ALL;

        var countEdit = this.countEdit = InspectorUtils.createAttrEditField(title, this);

        InspectorUtils.createSeparator(this);

        this.countEditField = <Atomic.UIEditField> countEdit.getWidget("editfield");

        this.subscribeToEvent(this.countEditField, "WidgetEvent", (data) => this.handleCountWidgetEvent(data));
        this.subscribeToEvent(this.countEditField, "WidgetFocusChanged", (data) => this.handleCountWidgetFocusChanged(data));

    }

    handleCountWidgetFocusChanged(ev) {

        if (ev.focused) {

            this.countRestore = this.countEditField.text;

        } else {

            this.countEditField.text = this.countRestore;

        }

    }

    handleCountWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_KEY_UP) {

            if (ev.key == Atomic.KEY_RETURN) {

                if (this.countRestore != this.countEditField.text) {

                    this.countRestore = this.countEditField.text;

                    if (this.onCountChanged) {

                      this.onCountChanged(Number(this.countRestore));

                    }

                }

            }

        }

        if (ev.type == Atomic.UI_EVENT_TYPE_CHANGED) {

        }

    }

    countEdit:Atomic.UIEditField;

    onCountChanged: (count:number) => void;

    countRestore: string;
    countEditField: Atomic.UIEditField;

}

export = ArrayEditWidget;
