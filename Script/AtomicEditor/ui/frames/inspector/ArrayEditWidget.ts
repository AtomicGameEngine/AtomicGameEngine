//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
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
