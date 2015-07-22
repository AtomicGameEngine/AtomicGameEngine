
import EditorEvents = require("../../editor/EditorEvents");

class PlayMode extends Atomic.ScriptObject {

  constructor() {

    super();

    this.subscribeToEvent("IPCJSError", (ev:Atomic.IPCJSErrorEvent) => this.handleIPCJSError(ev));

  }

  handleIPCJSError(ev:Atomic.IPCJSErrorEvent) {

    var errorMessage = ev.errorFileName + " - " + ev.errorLineNumber + " : " + ev.errorMessage;
    this.sendEvent(EditorEvents.ModalError, { title: "Player JavaScript Error", message: errorMessage});

    Atomic.graphics.raiseWindow();

  }

}

export = PlayMode;
