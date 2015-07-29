
import EditorUI = require("../EditorUI");

class ProgressModal extends Atomic.UIWindow {

    constructor(title: string, message: string) {

        super();

        this.settings = Atomic.UI_WINDOW_SETTINGS_DEFAULT & ~Atomic.UI_WINDOW_SETTINGS_CLOSE_BUTTON;

        this.text = title;
        this.load("AtomicEditor/editor/ui/progressmodal.tb.txt");

        var messageField = <Atomic.UITextField> this.getWidget("message");
        messageField.text = message;

        this.resizeToFitContent();
        this.center();

        this.dimmer = new Atomic.UIDimmer();

    }

    show() {

        var view = EditorUI.getView();
        view.addChild(this.dimmer);
        view.addChild(this);

    }

    hide() {

        if (this.dimmer.parent)
            this.dimmer.parent.removeChild(this.dimmer, false);

        if (this.parent)
            this.parent.removeChild(this, false);

    }

    dimmer: Atomic.UIDimmer;

}

export = ProgressModal;
