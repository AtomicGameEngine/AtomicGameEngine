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

    constructor(showCheck:boolean = true) {

        super();

        this.settings = Atomic.UI_WINDOW_SETTINGS.UI_WINDOW_SETTINGS_DEFAULT & ~Atomic.UI_WINDOW_SETTINGS.UI_WINDOW_SETTINGS_CLOSE_BUTTON;

        // we're not calling this.init here as it calls resizeToFitContent
        // and center, which screw up the generated About text being resized

        this.text = showCheck ? "New Build Detected" : "Latest Updates";
        this.load("AtomicEditor/editor/ui/newbuildwindow.tb.txt");

        this.newbuild_text = <Atomic.UIEditField>this.getWidget("newbuild_text");
        this.newbuild_text.text = this.generateNewBuildText();

        this.newBuildCheck = <Atomic.UICheckBox> this.getWidget("newbuild_check");

        if (!showCheck) {

            this.getWidget("newbuild_check_text").visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
            this.newBuildCheck.visibility = Atomic.UI_WIDGET_VISIBILITY.UI_WIDGET_VISIBILITY_GONE;
        }

        this.resizeToFitContent();
        this.center();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE.UI_EVENT_TYPE_CLICK) {

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

Atomic Build 2 - <color #AAAAAA>(Git SHA: ${Atomic.AtomicBuildInfo.getGitSHA()})</color>

<color #D4FB79>Closed Issues</color>

<widget TBButton: text: 'https://github.com/AtomicGameEngine/AtomicGameEngine/milestone/3' url: 'https://github.com/AtomicGameEngine/AtomicGameEngine/milestone/3?closed=1' skin: TBButton.link>

<color #D4FB79>Update Highlights</color>

• <color #D4FB79>[Profiling]</color> Added Object memory metrics profiling subsystem

• <color #D4FB79>[C#]</color> CSComponent inspector support for dynamic/fixed size array members

• <color #D4FB79>[Player]</color> Player subsystem Scene handling improvements 

• <color #D4FB79>[Wiki]</color> Many additions and improvements to the Atomic Wiki!

• <color #D4FB79>[Script]</color> Exposed VariantVector, Frustum, PackageFile, and UI::CycleDebugHudMode to script

• <color #D4FB79>[Script]</color> Improvements to ScriptVector and ScriptVariant

• <color #D4FB79>[TypeScript]</color> Updated to use generated native enum values

• <color #D4FB79>[TypeScript]</color> Constants converted to enums where possible 

• <color #D4FB79>[Platform]</color> Added additional SDL key mappings

• <color #D4FB79>[Editor]</color> Project load/save now remembers the last folder used

• <color #D4FB79>[Editor]</color> Updated Welcome screen and help menu with new Atomic Support forums

• <color #D4FB79>[Linux]</color> Fixed issue with Pulse audio CPU usage and possible black screen

• <color #D4FB79>[C#]</color> Fixed issue with custom C# script events

• <color #D4FB79>[C#]</color> Fixed issue with reparenting C# nodes and component updates

• <color #D4FB79>[Editor]</color> Fixed issue with multiple selected resource tabs

• <color #D4FB79>[Editor]</color> Improved contrast on example language toggle button states 

• <color #D4FB79>[ToolCore]</color> Fixed issues with JSONFile::GetRoot in assets

• <color #D4FB79>[Deployment]</color> Removed some UI psd assets that were in deployments 

`;

        return text;

    }

    newbuild_text: Atomic.UIEditField;
    newBuildCheck: Atomic.UICheckBox;

}

export = NewBuildWindow;
