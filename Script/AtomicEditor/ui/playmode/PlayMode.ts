//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("../../editor/EditorEvents");
import PlayerOutput = require("./PlayerOutput");

class PlayMode extends Atomic.ScriptObject {

    inErrorState: boolean;

    constructor() {

        super();

        this.subscribeToEvent("IPCJSError", (ev: Atomic.IPCJSErrorEvent) => this.handleIPCJSError(ev));
        this.subscribeToEvent(EditorEvents.PlayerStarted, (ev) => this.handlePlayerStarted(ev));

    }

    handlePlayerStarted(ev) {

        this.inErrorState = false;

        new PlayerOutput();
    }

    handleIPCJSError(ev: Atomic.IPCJSErrorEvent) {

        if (this.inErrorState)
            return;

        this.inErrorState = true;

        var errorMessage = ev.errorFileName + " - " + ev.errorLineNumber + " : " + ev.errorMessage;
        this.sendEvent(EditorEvents.ModalError, { title: "Player JavaScript Error", message: errorMessage });

        Atomic.graphics.raiseWindow();

    }

}

export = PlayMode;
