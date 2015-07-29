
import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");
import ScriptWidget = require("./ScriptWidget");

class WelcomeFrame extends ScriptWidget {

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/welcomeframe.tb.txt");

        this.recentProjects = <Atomic.UISelectList> this.getWidget("recentprojects");
        this.recentSource = new Atomic.UISelectItemSource();
        this.gravity = Atomic.UI_GRAVITY_ALL;

        var container = <Atomic.UILayout> parent.getWidget("resourceviewcontainer");

        container.addChild(this);

        this.updateRecentProjects();

    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

            if (id == "recentprojects") {

                var itemID = Number(this.recentProjects.selectedItemID);

                if (itemID >= 0 && itemID < this.recent.length) {

                    this.sendEvent(EditorEvents.LoadProject, { path: this.recent[itemID] });

                }

                return true;

            }

            if (id == "open project") {

                var utils = new Editor.FileUtils();
                var path = utils.openProjectFileDialog();
                if (path) {

                    this.sendEvent(EditorEvents.LoadProject, { path: path });

                }

                return true;
            }

            if (id == "new project") {

                var mo = EditorUI.getModelOps();
                mo.showNewProject();
                return true;

            }

        }

    }

    updateRecentProjects() {

        this.recentSource.clear();

        // prune any that don't exist
        Atomic.editorMode.preferences.updateRecentFiles();

        this.recent = Atomic.editorMode.preferences.recentProjects;

        for (var i in this.recent) {

            this.recentSource.addItem(new Atomic.UISelectItem(this.recent[i], i))

        }

        this.recentProjects.source = this.recentSource;

    }

    recent: string[] = [];
    recentProjects: Atomic.UISelectList;
    recentSource: Atomic.UISelectItemSource;

}

export = WelcomeFrame;
