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

import EditorUI = require("../../EditorUI");


class GlowOutput extends Atomic.UIWindow {

    constructor() {

        super();

        this.settings = Atomic.UI_WINDOW_SETTINGS.UI_WINDOW_SETTINGS_DEFAULT & ~Atomic.UI_WINDOW_SETTINGS.UI_WINDOW_SETTINGS_CLOSE_BUTTON;

        this.text = "Atomic Glow";
        this.load("AtomicEditor/editor/ui/atomicglowoutput.tb.txt");

        this.outputField = <Atomic.UIEditField>this.getWidget("output");

        this.resizeToFitContent();
        this.center();

        this.subscribeToEvent(Editor.AtomicGlowLogEvent((ev: Editor.AtomicGlowLogEvent) => {

            this.textOutput += ev.message + "\n";
            this.outputField.text = this.textOutput;
            this.outputField.scrollTo(0, 0xffffff);

        }));

        this.subscribeToEvent(this, Atomic.UIWidgetEvent((data) => this.handleWidgetEvent(data)));

        this.dimmer = new Atomic.UIDimmer();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "cancel") {
                this.sendEvent(Editor.AtomicGlowBakeCancelEventType);
                return true;
            }

            if (ev.target.id == "ok") {
                return true;
            }

        }

        return false;
    }

    show() {

        var view = EditorUI.getView();
        view.addChild(this.dimmer);
        view.addChild(this);

    }

    hide() {

        this.unsubscribeFromEvent(Editor.AtomicGlowLogEventType);

        if (this.dimmer.parent)
            this.dimmer.parent.removeChild(this.dimmer, false);

        if (this.parent)
            this.parent.removeChild(this, false);

    }

    dimmer: Atomic.UIDimmer;
    
    textOutput: string = "";
    outputField: Atomic.UIEditField;

}

export = GlowOutput;
