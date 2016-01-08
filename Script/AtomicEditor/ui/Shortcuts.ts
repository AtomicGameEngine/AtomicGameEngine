//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");
import Preferences = require("editor/Preferences")

class Shortcuts extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent("UIShortcut", (ev: Atomic.UIShortcutEvent) => this.handleUIShortcut(ev));

        this.subscribeToEvent("KeyDown", (ev: Atomic.KeyDownEvent) => this.handleKeyDown(ev));


    }

    //this should be moved somewhere else...
    invokePlayOrStopPlayer(debug:boolean = false) {
        this.sendEvent(EditorEvents.SaveAllResources);
        if (Atomic.editorMode.isPlayerEnabled()) {
            this.sendEvent("IPCPlayerExitRequest");
        } else {
            var playerWindow = Preferences.getInstance().playerWindow;
            if (playerWindow) {
                if ((playerWindow.monitor + 1) > Atomic.graphics.getMonitorsNumber()) {
                    //will use default settings if monitor is not available
                    var args = "--resizable";
                    Atomic.editorMode.playProject(args, debug);
                } else {
                    var args = "--windowposx " + playerWindow.x + " --windowposy " + playerWindow.y + " --windowwidth " + playerWindow.width + " --windowheight " + playerWindow.height + " --resizable";
                    if (playerWindow.maximized) {
                        args += " --maximize";
                    }
                    Atomic.editorMode.playProject(args, debug);
                }
            } else {
                Atomic.editorMode.playProject("", debug);
            }
        }
    }

    invokeFormatCode() {

        var editor = EditorUI.getCurrentResourceEditor();

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

    invokeFrameSelected() {
        this.invokeResourceFrameShortcut("frameselected");
    }


    invokeSelectAll() {
        this.invokeResourceFrameShortcut("selectall");
    }

    invokeGizmoEditModeChanged(mode: Editor.EditMode) {

        this.sendEvent("GizmoEditModeChanged", { mode: mode });

    }

    toggleGizmoAxisMode() {
        var editor = EditorUI.getCurrentResourceEditor();

        if (editor && editor instanceof Editor.SceneEditor3D) {
            var mode = editor.getGizmo().axisMode ? Editor.AXIS_WORLD : Editor.AXIS_LOCAL;
            this.sendEvent("GizmoAxisModeChanged", { mode: mode });
        }
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

            if (!Atomic.ui.focusedWidget && !this.cmdKeyDown()) {

                if (ev.key == Atomic.KEY_W) {
                    this.invokeGizmoEditModeChanged(Editor.EDIT_MOVE);
                } else if (ev.key == Atomic.KEY_E) {
                    this.invokeGizmoEditModeChanged(Editor.EDIT_ROTATE);
                } else if (ev.key == Atomic.KEY_R) {
                    this.invokeGizmoEditModeChanged(Editor.EDIT_SCALE);
                } else if (ev.key == Atomic.KEY_X) {
                    this.toggleGizmoAxisMode();
                } else if (ev.key == Atomic.KEY_F) {
                    this.invokeFrameSelected();
                }
            }

        }

    }

    cmdKeyDown(): boolean {

        var cmdKey;
        if (Atomic.platform == "MacOSX") {
            cmdKey = (Atomic.input.getKeyDown(Atomic.KEY_LGUI) || Atomic.input.getKeyDown(Atomic.KEY_RGUI));
        } else {
            cmdKey = (Atomic.input.getKeyDown(Atomic.KEY_LCTRL) || Atomic.input.getKeyDown(Atomic.KEY_RCTRL));
        }

        return cmdKey;


    }

    // global shortcut handler
    handleUIShortcut(ev: Atomic.UIShortcutEvent) {

        var cmdKey = this.cmdKeyDown();

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
                this.invokePlayOrStopPlayer();
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
