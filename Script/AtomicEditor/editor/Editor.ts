
import MainFrame = require("../ui/MainFrame");
import UIEvents = require("../ui/UIEvents");
import AssetImport = require("../assets/AssetImport");

import EditorEvents = require("./EditorEvents");

class Editor extends Atomic.ScriptObject {

    project: ToolCore.Project;
    view: Atomic.UIView;
    mainframe: MainFrame;
    assetImport: AssetImport;

    static instance: Editor;

    constructor() {

        super();

        Editor.instance = this;

        Atomic.getResourceCache().autoReloadResources = true;

        this.assetImport = new AssetImport();

        var graphics = Atomic.getGraphics();

        this.view = new Atomic.UIView();

        this.mainframe = new MainFrame();

        this.view.addChild(this.mainframe);

        // set initial size
        this.mainframe.setSize(graphics.width, graphics.height);

        this.parseArguments();

        this.subscribeToEvent(EditorEvents.Quit, (data) => this.handleEditorEventQuit(data));

    }

    loadProject(projectPath: string): boolean {

        var system = ToolCore.getToolSystem();

        if (system.project) {

            this.sendEvent(UIEvents.MessageModalEvent,
                { type: "error", title: "Project already loaded", message: "Project already loaded" });

            return false;

        }

        return system.loadProject(projectPath);

    }

    parseArguments() {

        var args = Atomic.getArguments();

        var idx = 0;

        while (idx < args.length) {

            if (args[idx] == "--project") {

                this.loadProject(args[idx + 1]);

            }

            idx++;

        }

    }

    // event handling

    handleEditorEventQuit(data) {

      Atomic.getEngine().exit();

    }


}

export = Editor;
