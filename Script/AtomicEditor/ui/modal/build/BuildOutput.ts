//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
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
