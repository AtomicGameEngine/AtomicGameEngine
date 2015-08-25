
import EditorUI = require("../EditorUI");

class ModalWindow extends Atomic.UIWindow {

    constructor(disableClose:boolean = false) {

        super();

        if (disableClose)
          this.settings = Atomic.UI_WINDOW_SETTINGS_DEFAULT & ~Atomic.UI_WINDOW_SETTINGS_CLOSE_BUTTON;

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
