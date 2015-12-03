//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import InspectorUtils = require("./InspectorUtils");

class ArrayEditWidget extends Atomic.UILayout {

    constructor(title: string) {

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

        this.countEditField = <Atomic.UIEditField>countEdit.getWidget("editfield");

        this.subscribeToEvent(this.countEditField, "UIWidgetEditComplete", (ev) => this.handleUIWidgetEditCompleteEvent(ev));

    }

    handleUIWidgetEditCompleteEvent(ev):boolean {

        var count = Number(this.countEditField.text);

        if (this.onCountChanged) {

            this.onCountChanged(count);

        }

        return true;

    }

    countEdit: Atomic.UIEditField;

    onCountChanged: (count: number) => void;

    countEditField: Atomic.UIEditField;

}

export = ArrayEditWidget;
