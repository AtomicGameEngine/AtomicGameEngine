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

            this.getWidget("newbuild_check_text").visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
            this.newBuildCheck.visibility = Atomic.UI_WIDGET_VISIBILITY_GONE;
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

<widget TBButton: text: 'https://github.com/AtomicGameEngine/AtomicGameEngine/milestone/1' url: 'https://github.com/AtomicGameEngine/AtomicGameEngine/milestone/1?closed=1' skin: TBButton.link>

<color #D4FB79>Update Highlights</color>

• <color #D4FB79>[Editor]</color> Custom file resource inspector plugins (with example)

• <color #D4FB79>[Editor]</color> Added TmxFile2D resource type for inspector fields

• <color #D4FB79>[Docs]</color> Added new C#, C++, and updated JavaScript/TypeScript API references

• <color #D4FB79>[Network]</color> Restored functionality for master server and client

• <color #D4FB79>[Web]</color> Added Web subsystem events and convenience methods for post data and responses

• <color #D4FB79>[C#]</color> Output dev project assemblies to Lib, so when modifying AtomicNET sources, changes are used properly

• <color #D4FB79>[C#]</color> CSComponent cleanups for instantiation from script/serialized from scene (also cleans up nativeOverride hack)

• <color #D4FB79>[C#]</color> Fix for exception when instantiating any RefCounted derived instance during a CSComponent default constructor

• <color #D4FB79>[C#]</color> Better error reporting for CSComponent load issues

• <color #D4FB79>[C#]</color> Added Material.SetShaderParameter API

• <color #D4FB79>[C#]</color> Added Vector4/String to ScriptVariant

• <color #D4FB79>[C#]</color> On demand project assembly compilation from within the Atomic Editor

• <color #D4FB79>[C#]</color> Inspector attribute can now be used to set inspector tooltips

• <color #D4FB79>[TypeScript]</color> Upgraded to TypeScript 2.1

• <color #D4FB79>[TypeScript]</color> Removed deprecated allowNonTsExtensions

• <color #D4FB79>[TypeScript]</color> Automatically generate a tasks.json for VSCode

• <color #D4FB79>[TypeScript]</color> Updated tsconfig with rootUrl properly for non-relative imports

• <color #D4FB79>[TypeScript]</color> Strongly typed native event interfaces and subscription

• <color #D4FB79>[Examples]</color> Fixed exception with virtual dpad in JavaScript examples

• <color #D4FB79>[Desktop]</color> Fixed issues with engine configuration json parsing in deployed applications

• <color #D4FB79>[Windows]</color> Fixed issue with Visual Studio 2017 detection

• <color #D4FB79>[Windows]</color> Fixed UI pixel offset issue when rendering with OpenGL

• <color #D4FB79>[macOS]</color> Added NSHighResolutionCapable flag to Atomic Editor

• <color #D4FB79>[General]</color> Updated About dialog with contributor and build vendor information

• <color #D4FB79>[General]</color> Misc bug fixes and optimizations

• <color #D4FB79>[Maintenance]</color> Removed CurlManager from ToolCore as duplicated Web subsystem

`;

        return text;

    }

    newbuild_text: Atomic.UIEditField;
    newBuildCheck: Atomic.UICheckBox;

}

export = NewBuildWindow;
