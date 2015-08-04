
import EditorEvents = require("editor/EditorEvents");
import EditorUI = require("ui/EditorUI");
import ScriptWidget = require("ui/ScriptWidget");
import Preferences = require("editor/Preferences");

class WelcomeFrame extends ScriptWidget {

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/welcomeframe.tb.txt");

        var recentProjects = <Atomic.UILayout> this.getWidget("recentprojects");
        this.gravity = Atomic.UI_GRAVITY_ALL;

        this.recentList = new Atomic.UIListView();
        this.recentList.rootList.id = "recentList";

        recentProjects.addChild(this.recentList);

        var container = <Atomic.UILayout> parent.getWidget("resourceviewcontainer");

        container.addChild(this);

        this.updateRecentProjects();

        this.subscribeToEvent(EditorEvents.CloseProject, () => {
            this.updateRecentProjects();
        });
        
    }

    handleWidgetEvent(ev: Atomic.UIWidgetEvent) {
        if (ev.type == Atomic.UI_EVENT_TYPE_RIGHT_POINTER_UP) {
            if (ev.target.id == "recentList") {
                this.openFrameMenu(ev.x, ev.y);
            }
        }

        if (ev.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var id = ev.target.id;

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

            if (id == "recentList") {
                var path: string = this.recent[this.recentList.getSelectedItemID()];
                this.sendEvent(EditorEvents.LoadProject, { path: path });
            }

            if (id == "recentProjectsContextMenu") {
                var prefs = Preferences.getInstance();
                if (ev.refid == "clear recent projects") {
                    prefs.deleteRecentProjects();
                    this.updateRecentProjects();
                }
            }
        }
    }

    updateRecentProjects() {

        this.recentList.deleteAllItems();
        
        var prefs = Preferences.getInstance();
        prefs.updateRecentProjects();
        
        this.recent = prefs.recentProjects;

        for (var i in this.recent) {
            this.recentList.addRootItem(this.recent[i], "Folder.icon", i);
        }

    }

    private openFrameMenu(x: number, y: number) {
        var menu = new Atomic.UIMenuWindow(this, "recentProjectsContextMenu");
        var menuButtons = new Atomic.UISelectItemSource();
        menuButtons.addItem(new Atomic.UISelectItem("Clear Recent Projects", "clear recent projects"));
        menu.show(menuButtons, x, y);
    }

    recent: string[] = [];
    recentList: Atomic.UIListView;

}

export = WelcomeFrame;
