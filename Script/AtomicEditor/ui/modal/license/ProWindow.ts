
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ModalWindow = require("../ModalWindow");
import ProgressModal = require("../ProgressModal");

class ProWindow extends ModalWindow {

    constructor(uiPath: string) {

        super(true);

        this.init("Atomic Pro Required", uiPath);

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == 'purchase') {

                Atomic.fileSystem.systemOpen("https://store.atomicgameengine.com/site");

            } else if (id == "download") {

                Atomic.fileSystem.systemOpen("http://atomicgameengine.com/download");

            } else if (id == "quit") {

                this.sendEvent(EditorEvents.Quit);

            }

            if (id == "ok") {

                this.hide();

                return true;
            }

            return false;

        }
    }

}

export = ProWindow;
