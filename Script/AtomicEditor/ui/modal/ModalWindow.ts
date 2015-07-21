
import EditorUI = require("../EditorUI");

class ModalWindow extends Atomic.UIWindow {

    constructor() {

        super();

        var view = EditorUI.getView();
        view.addChild(this);

        this.setFocus();

        this.subscribeToEvent(this, "WidgetDeleted", (event: Atomic.UIWidgetDeletedEvent) => {

            this.hide();

        });

        this.subscribeToEvent(this, "WidgetEvent", (data) => this.handleWidgetEvent(data));

    }

    hide() {

        var modalOps = EditorUI.getModelOps();
        modalOps.hide();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

    }

    init(windowText: string, uifilename: string) {

        this.text = windowText;
        this.load(uifilename);
        this.resizeToFitContent();
        this.center();

    }

}

export = ModalWindow;
