import EditorUI = require("../EditorUI");
import ModalWindow = require("./ModalWindow");
import NewProject = require("./NewProject");
import CreateProject = require("./CreateProject");

class ModalOps extends Atomic.ScriptObject {

    constructor() {

        super();

        this.dimmer = new Atomic.UIDimmer();

    }

    showCreateProject(projectTemplateFolder:string) {

      if (this.show()) {

          this.opWindow = new CreateProject(projectTemplateFolder);

      }

    }

    showNewProject() {

        if (this.show()) {

            this.opWindow = new NewProject();

        }
    }

    private show(): boolean {

        if (this.dimmer.parent) {

            console.log("WARNING: attempting to show modal while dimmer is active");
            return false;

        }

        if (this.opWindow) {

            console.log("WARNING: attempting to show modal while another opWindow is active");
            return false;

        }

        var view = EditorUI.getView();
        view.addChild(this.dimmer);

        return true;

    }

    hide() {

        if (this.opWindow) {

            var window = this.opWindow;
            this.opWindow = null;

            if (window.parent)
              window.parent.removeChild(window, false);

            var view = EditorUI.getView();
            view.setFocusRecursive();

        }

        if (this.dimmer.parent) {

            this.dimmer.parent.removeChild(this.dimmer, false);

        }


    }

    dimmer: Atomic.UIDimmer;
    opWindow: ModalWindow;

}

export = ModalOps;
