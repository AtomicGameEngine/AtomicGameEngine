
import UIEvents = require("../UIEvents");
import EditorUI = require("../EditorUI");

export class MessageModal extends Atomic.ScriptObject
{

  showErrorWindow(title:string, message:string):void {

    var mainframe = EditorUI.getMainFrame();

    new Atomic.UIMessageWindow(mainframe, "modal_error").show(title, message, Atomic.UI_MESSAGEWINDOW_SETTINGS_OK, true, 640, 360);

  }

  constructor() {

    super();

    this.subscribeToEvent(UIEvents.MessageModalEvent, (data) => {

      if (data.type == "error") {

        this.showErrorWindow(data.title, data.message);

      }

    });

  }

}
