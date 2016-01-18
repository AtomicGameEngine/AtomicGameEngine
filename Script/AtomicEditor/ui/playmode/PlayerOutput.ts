//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("../../editor/EditorEvents");
import EditorUI = require("../EditorUI");

class PlayerOutput extends Atomic.UIWindow {

    output: Atomic.UIEditField;

    constructor() {

        super();

        var view = EditorUI.getView();
        view.addChild(this);

        this.text = "Player Output";

        this.load("AtomicEditor/editor/ui/playeroutput.tb.txt");

        this.output = <Atomic.UIEditField> this.getWidget("output");

        (<Atomic.UIButton>this.getWidget("closebutton")).onClick = () => {

            this.close();

        };

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));
        this.subscribeToEvent(EditorEvents.PlayerLog, (ev: EditorEvents.PlayerLogEvent) => this.handlePlayerLog(ev));

        this.resizeToFitContent();
        this.center();
        this.setFocus();

    }

    handlePlayerLog(ev: EditorEvents.PlayerLogEvent) {

        var text = this.output.text;

        if (text.length > 32768)
            this.output.text = "";

        this.output.appendText(ev.message + "\n");
        this.output.scrollTo(0, 0xffffff);
    }


    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

    }

}

export = PlayerOutput;
