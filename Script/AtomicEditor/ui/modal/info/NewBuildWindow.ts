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

        this.text = "New Build Detected";
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

                    Preferences.getInstance().editorBuildData.lastEditorBuildSHA = Atomic.AtomicBuildInfo.getGitSHA();
                    Preferences.getInstance().write();

                }
                return true;
            }
        }
    }


    generateNewBuildText(): string {

        var text = "";

        // TODO: this needs to be externally generated
        text += `
<color #D4FB79>Hello and thanks for installing a new build of the Atomic Editor!</color>
<color #AAAAAA>(Git SHA: ${Atomic.AtomicBuildInfo.getGitSHA()})</color>

<color #76D6FF>Latest Updates:</color>

• Support for <color #D4FB79>Atomic C#</color> scripting on Windows, macOS, Linux, Android, and iOS

• <color #D4FB79>Visual Studio</color> and <color #D4FB79>Xamarin Studio</color> integration (with VSCode support coming soon!)

• Physically Based Rendering (PBR) - Thanks to @dragonCASTjosh

• New examples and project templates

• Revamped build targeting JavaScript, TypeScript, C#, and native C++

• Updated to Urho3D 1.6 and SDL 2.0.4 with a great number of platform improvements

• Updated to to Monaco VSCode editor 0.6

• Bug fixed, improvements, and optimization
`;

        return text;

    }

    newbuild_text: Atomic.UIEditField;
    newBuildCheck: Atomic.UICheckBox;

}

export = NewBuildWindow;
