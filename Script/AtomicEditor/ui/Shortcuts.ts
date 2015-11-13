//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");

class Shortcuts extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent("UIShortcut", (ev: Atomic.UIShortcutEvent) => this.handleUIShortcut(ev));

        this.subscribeToEvent("KeyDown", (ev: Atomic.KeyDownEvent) => this.handleKeyDown(ev));


    }

    invokePlay() {

        this.sendEvent(EditorEvents.SaveAllResources);
        Atomic.editorMode.playProject();

    }

    invokePlayDebug() {

        this.sendEvent(EditorEvents.SaveAllResources);
        Atomic.editorMode.playProjectDebug();

    }


    invokeFormatCode() {

        var editor = EditorUI.getMainFrame().resourceframe.currentResourceEditor;

        if (editor && editor.typeName == "JSResourceEditor") {

            (<Editor.JSResourceEditor>editor).formatCode();

        }

    }

    invokeFileClose() {
        this.invokeResourceFrameShortcut("close");
    }

    invokeFileSave() {
        this.sendEvent(EditorEvents.SaveResource);
    }

    invokeUndo() {
        this.invokeResourceFrameShortcut("undo");
    }

    invokeRedo() {
        this.invokeResourceFrameShortcut("redo");
    }

    invokeCut() {
        this.invokeResourceFrameShortcut("cut");
    }

    invokeCopy() {
        this.invokeResourceFrameShortcut("copy");
    }

    invokePaste() {
        this.invokeResourceFrameShortcut("paste");
    }

    invokeSelectAll() {
        this.invokeResourceFrameShortcut("selectall");
    }

    invokeGizmoEditModeChanged(mode:Editor.EditMode) {

        this.sendEvent("GizmoEditModeChanged", { mode: mode });

    }

    invokeGizmoAxisModeChanged(mode:Editor.AxisMode, toggle:boolean = false) {

        this.sendEvent("GizmoAxisModeChanged", { mode: mode, toggle: toggle });

    }

    invokeResourceFrameShortcut(shortcut: string) {
        if (!ToolCore.toolSystem.project) return;
        var resourceFrame = EditorUI.getMainFrame().resourceframe.currentResourceEditor;
        if (resourceFrame) {
            resourceFrame.invokeShortcut(shortcut);
        }
    }

    handleKeyDown(ev: Atomic.KeyDownEvent) {

        // if the right mouse buttons isn't down
        if (!(ev.buttons & Atomic.MOUSEB_RIGHT)) {

            // TODO: Make these customizable

            if (ev.key == Atomic.KEY_W) {
                this.invokeGizmoEditModeChanged(Editor.EDIT_MOVE);
            } else if (ev.key == Atomic.KEY_E) {
              this.invokeGizmoEditModeChanged(Editor.EDIT_ROTATE);
            } else if (ev.key == Atomic.KEY_R) {
                this.invokeGizmoEditModeChanged(Editor.EDIT_SCALE);
            } else if (ev.key == Atomic.KEY_X) {
                this.invokeGizmoAxisModeChanged(Editor.AXIS_WORLD, true);
            }

        }

    }

    // global shortcut handler
    handleUIShortcut(ev: Atomic.UIShortcutEvent) {

        var cmdKey;
        if (Atomic.platform == "MacOSX") {
            cmdKey = (Atomic.input.getKeyDown(Atomic.KEY_LGUI) || Atomic.input.getKeyDown(Atomic.KEY_RGUI));
        } else {
            cmdKey = (Atomic.input.getKeyDown(Atomic.KEY_LCTRL) || Atomic.input.getKeyDown(Atomic.KEY_RCTRL));
        }

        if (cmdKey) {

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
                //if shift is pressed
            } else if (ev.qualifiers & Atomic.QUAL_SHIFT) {
                if (ev.key == Atomic.KEY_B) {
                    EditorUI.getModelOps().showBuildSettings();
                }
            } else if (ev.key == Atomic.KEY_B) {
                EditorUI.getModelOps().showBuild();
            }

        }

    }

}

export = Shortcuts;
