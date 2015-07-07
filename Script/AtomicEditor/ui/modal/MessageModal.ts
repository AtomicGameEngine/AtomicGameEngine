
import UIEvents = require("../UIEvents");
import Editor = require("../../editor/Editor");

export class MessageModal extends Atomic.ScriptObject
{

  showErrorWindow(title:string, message:string):void {

    var mainframe = Editor.getEditor().mainframe;

    new Atomic.UIMessageWindow(mainframe, "modal_error").show(title, message, 640, 360);

  }

  constructor() {

    super();

    // Subscribe to graphics subsystems screen mode switching, so we can adjust the widget size
    this.subscribeToEvent(UIEvents.MessageModalEvent, (data) => {

      if (data.type == "error") {

        this.showErrorWindow(data.title, data.message);

      }

    });


  }

}
