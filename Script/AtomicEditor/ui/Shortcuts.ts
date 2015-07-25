import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");

class Shortcuts extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent("UIShortcut", (ev: Atomic.UIShortcutEvent) => this.handleUIShortcut(ev));


    }

    invokePlay() {

        this.sendEvent(EditorEvents.SaveAllResources);
        Atomic.editorMode.playProject();

    }

    invokeFormatCode() {

        var editor = EditorUI.getMainFrame().resourceframe.currentResourceEditor;

        if (editor && editor.typeName == "JSResourceEditor") {

            (<Editor.JSResourceEditor>editor).formatCode();

        }

    }

    invokeFileClose() {

        // pretty gross
        var editor = EditorUI.getMainFrame().resourceframe.currentResourceEditor;
        editor.close(true);

    }

    invokeFileSave() {
        this.sendEvent(EditorEvents.SaveResource);
    }


    // global shortcut handler
    handleUIShortcut(ev: Atomic.UIShortcutEvent) {

        // global shortcuts without qualifiers
        if (!ev.qualifiers) {

            if (ev.key == Atomic.KEY_S) {
                this.invokeFileSave();
            }
            else if (ev.key == Atomic.KEY_W) {
                this.invokeFileClose();
            }
            else if (ev.key == Atomic.KEY_I) {
                this.invokeFormatCode();
            }

            else if (ev.key == Atomic.KEY_P) {
                this.invokePlay();
            }

        }

    }

}

export = Shortcuts;
