//
// Copyright (c) 2014-2016 THUNDERBEAST GAMES LLC
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.
//

import EditorEvents = require("../editor/EditorEvents");
import EditorUI = require("./EditorUI");
import Preferences = require("editor/Preferences");

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
                if ((playerWindow.monitor + 1) > Atomic.graphics.getNumMonitors()) {
                    //will use default settings if monitor is not available
                    var args = "--resizable";
                    Atomic.editorMode.playProject(args, debug);
                } else {
                    var args = "--windowposx " + playerWindow.x + " --windowposy " + playerWindow.y + " --windowwidth " + playerWindow.width + " --windowheight " + playerWindow.height + " --resizable" + " --fromEditorPlay";
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

    invokePauseOrResumePlayer() {
        if (Atomic.editorMode.isPlayerEnabled()) {
            this.sendEvent("IPCPlayerPauseResumeRequest");
        }
    }

    invokeStepPausedPlayer() {
        if (Atomic.editorMode.isPlayerEnabled()) {
            this.sendEvent("IPCPlayerPauseStepRequest");
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
            } else if (ev.key == Atomic.KEY_B) {
                if (ev.qualifiers & Atomic.QUAL_SHIFT) {
                    EditorUI.getModelOps().showBuildSettings();
                } else {
                    EditorUI.getModelOps().showBuild();
                }
            }
            else if (ev.key == Atomic.KEY_U) {
                if (ev.qualifiers & Atomic.QUAL_SHIFT) {
                    this.invokeStepPausedPlayer();
                } else {
                    this.invokePauseOrResumePlayer();
                }
            }

        }

    }

}

export = Shortcuts;
