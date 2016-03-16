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

class BuildComplete extends Atomic.UIWindow {

    constructor(parent: Atomic.UIWidget, ev: ToolCore.BuildCompleteEvent) {

        super();

        parent.addChild(this);

        this.buildFolder = ev.buildFolder;

        this.text = ev.success ? "Build Complete" : "Build Failed";
        this.load("AtomicEditor/editor/ui/buildcomplete.tb.txt");

        this.resizeToFitContent();
        this.center();

        var reveal = <Atomic.UIButton>this.getWidget("reveal");

        if (!ev.success)
            reveal.setState(Atomic.UI_WIDGET_STATE_DISABLED, true);


        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent): boolean {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            if (ev.target.id == "reveal") {

                console.log("REVEAL!");

                var utils = new Editor.FileUtils();
                utils.revealInFinder(this.buildFolder);

            }

            if (ev.target.id == "ok") {
                // passed up to build output window for now
                // which closes both
                return false;
            }

        }

        return false;
    }

    buildFolder: string;

}

export = BuildComplete;
