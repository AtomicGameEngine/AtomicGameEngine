
import EditorUI = require("../EditorUI");

class ModalWindow extends Atomic.UIWindow {

  constructor() {

    super();

    var view = EditorUI.getView();
    view.addChild(this);

    this.setFocus();

    this.subscribeToEvent(this, "WidgetDeleted", (event:Atomic.UIWidgetDeletedEvent) => {

      var modalOps = EditorUI.getModelOps();
      modalOps.hide();

    });

  }

}

export = ModalWindow;
