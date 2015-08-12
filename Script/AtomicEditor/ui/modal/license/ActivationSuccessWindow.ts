
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");

class ActivationSuccessWindow extends ModalWindow {

    constructor() {

        super();

        this.init("Product Activation Successful", "AtomicEditor/editor/ui/activationsuccess.tb.txt");
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "ok") {

                this.hide();

                return true;
            }

        }

    }

}

export = ActivationSuccessWindow;
