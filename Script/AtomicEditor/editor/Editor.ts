
import EditorUI = require("ui/EditorUI");
import UIEvents = require("ui/UIEvents");
import PlayMode = require("ui/playmode/PlayMode");
import EditorLicense = require("./EditorLicense");
import EditorEvents = require("./EditorEvents");
import Preferences = require("./Preferences");

class Editor extends Atomic.ScriptObject {

    project: ToolCore.Project;
    editorLicense: EditorLicense;
    playMode: PlayMode;

    static instance: Editor;

    constructor() {

        super();

        // limit the framerate to limit CPU usage
        Atomic.getEngine().maxFps = 60;

        Editor.instance = this;

        this.editorLicense = new EditorLicense();

        Preferences.getInstance().read();

        EditorUI.initialize();

        this.playMode = new PlayMode();

        Atomic.getResourceCache().autoReloadResources = true;

        this.subscribeToEvent(EditorEvents.LoadProject, (data) => this.handleEditorLoadProject(data));
        this.subscribeToEvent(EditorEvents.CloseProject, (data) => this.handleEditorCloseProject(data));
        this.subscribeToEvent("ProjectUnloaded", (data) => this.handleProjectUnloaded(data));
        this.subscribeToEvent(EditorEvents.Quit, (data) => this.handleEditorEventQuit(data));
        this.subscribeToEvent("ExitRequested", (data) => this.handleExitRequested(data));

        this.subscribeToEvent("ProjectLoaded", (data) => {
            Preferences.getInstance().registerRecentProject(data.projectPath);
        })

        this.parseArguments();
    }

    handleEditorLoadProject(event: EditorEvents.LoadProjectEvent): boolean {

        var system = ToolCore.getToolSystem();

        if (system.project) {

            this.sendEvent(UIEvents.MessageModalEvent,
                { type: "error", title: "Project already loaded", message: "Project already loaded" });

            return false;

        }

        return system.loadProject(event.path);

    }

    handleEditorCloseProject(event) {
        var system = ToolCore.getToolSystem();

        if (system.project) {

            system.closeProject();

        }

    }

    handleProjectUnloaded(event) {

        this.sendEvent(EditorEvents.ActiveSceneChange, { scene: null });



    }

    parseArguments() {

        var args = Atomic.getArguments();

        var idx = 0;

        while (idx < args.length) {

            if (args[idx] == "--project") {

                this.sendEvent(EditorEvents.LoadProject, { path: args[idx + 1] });

            }

            idx++;

        }

    }

    // event handling
    handleExitRequested(data) {
        Preferences.getInstance().write();
        EditorUI.shutdown();

    }

    handleEditorEventQuit(data) {

        this.sendEvent("ExitRequested");

    }


}

export = Editor;
