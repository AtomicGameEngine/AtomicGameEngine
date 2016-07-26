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
import Preferences = require("editor/Preferences");

class NewBuildWindow extends ModalWindow {

    constructor() {

        super();

        this.settings = Atomic.UI_WINDOW_SETTINGS_DEFAULT & ~Atomic.UI_WINDOW_SETTINGS_CLOSE_BUTTON;

        // we're not calling this.init here as it calls resizeToFitContent
        // and center, which screw up the generated About text being resized

        this.text = "New Atomic Editor Build Detected";
        this.load("AtomicEditor/editor/ui/newbuildwindow.tb.txt");

        this.newbuild_text = <Atomic.UIEditField>this.getWidget("newbuild_text");
        this.newbuild_text.text = this.generateNewBuildText();

        this.newBuildCheck = <Atomic.UICheckBox> this.getWidget("newbuild_check");

        this.resizeToFitContent();
        this.center();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "cfi") {

                Atomic.fileSystem.systemOpen("http://www.atomicgameengine.com/funding/");
            }

            if (id == "ok") {

                this.hide();

                if (this.newBuildCheck.value) {

                    Preferences.getInstance().editorBuildData.lastEditorBuildSHA = Atomic.getGitRevision();
                    Preferences.getInstance().write();

                }
                return true;
            }
        }
    }


    generateNewBuildText(): string {

        var text = "";

        text += `<widget TBImageWidget: filename: 'AtomicEditor/editor/images/atomic_logo.png'>

<color #D4FB79>Hello and thanks for installing a new build of the Atomic Editor!</color>

A cross platform game engine targeting 6 platforms is a lot of work! We need to raise funds in order to continue developing features such as integrated debugging, C# scripting, editor workflow improvements, platform updates, and to address a growing list of issues. There are also other project expenses such as CI hardware, web site updates, hosting costs, etc.

In addition to monetary funding, we need help reviewing pull requests, addressing open issues, providing support in chat, etc.

<color #D4FB79>Thanks for your support!</color>

<widget TBImageWidget: filename: 'AtomicEditor/editor/images/josh.jpg'> - Josh Engebretson on behalf of the Atomic Community
`;

        return text;

    }

    newbuild_text: Atomic.UIEditField;
    newBuildCheck: Atomic.UICheckBox;

}

export = NewBuildWindow;
