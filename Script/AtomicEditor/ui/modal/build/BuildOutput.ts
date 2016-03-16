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
import ModalWindow = require("../ModalWindow");
import BuildComplete = require("./BuildComplete");

class BuildOutput extends ModalWindow {

    constructor(buildBase: ToolCore.BuildBase) {

        super();

        this.buildBase = buildBase;

        this.init("Build Output", "AtomicEditor/editor/ui/programoutput.tb.txt");

        this.outputField = <Atomic.UIEditField>this.getWidget("output");

        this.resizeToFitContent();
        this.center();

        this.subscribeToEvent(buildBase, "BuildOutput", (ev: ToolCore.BuildOutputEvent) => {

            this.textOutput += ev.text;
            this.outputField.text = this.textOutput;
            this.outputField.scrollTo(0, 0xffffff);

        });

        this.subscribeToEvent("BuildComplete", (ev: ToolCore.BuildCompleteEvent) => {

            new BuildComplete(this, ev);

        });

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "cancel") {
                this.hide();
                return true;
            }

            if (ev.target.id == "ok") {
                this.hide();
                return true;
            }

        }

        return false;
    }

    textOutput: string = "";
    buildBase: ToolCore.BuildBase;
    outputField: Atomic.UIEditField;

}

export = BuildOutput;
