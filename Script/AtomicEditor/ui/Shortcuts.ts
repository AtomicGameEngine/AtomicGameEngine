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

import EditorUI = require("./EditorUI");
import Preferences = require("editor/Preferences");
import * as EditorEvents from "../editor/EditorEvents";

class Shortcuts extends Atomic.ScriptObject {

    constructor() {

        super();

        this.subscribeToEvent(Atomic.UIShortcutEvent((ev) => this.handleUIShortcut(ev)));

        this.subscribeToEvent(Atomic.KeyDownEvent((ev) => this.handleKeyDown(ev)));


    }

    //this should be moved somewhere else...
    invokePlayOrStopPlayer(debug: boolean = false) {

        this.sendEvent(Editor.EditorSaveAllResourcesEventType);

        if (Atomic.editorMode.isPlayerEnabled()) {
            this.sendEvent(EditorEvents.IPCPlayerExitRequestEventType);
        } else {

            var playerWindow = Preferences.getInstance().playerWindow;

            if (playerWindow) {

                if ((playerWindow.monitor + 1) > Atomic.graphics.getNumMonitors()) {
                    //will use default settings if monitor is not available
                    var args = "--resizable";
                    Atomic.editorMode.playProject(args, debug);

                } else {

                    if (playerWindow.width == 0 || playerWindow.height == 0) {

                        playerWindow.width = Atomic.graphics.width * .75;
                        // 16:9
                        playerWindow.height = playerWindow.width * .56;

                        let pos = Atomic.graphics.windowPosition;

                        playerWindow.x = pos[0] + (Atomic.graphics.width / 2 - playerWindow.width / 2);
                        playerWindow.y = pos[1] + (Atomic.graphics.height / 2 - playerWindow.height / 2);

                        // if too small a window, use default (which maximizes)
                        if (playerWindow.width < 480) {
                            playerWindow.width = playerWindow.height = playerWindow.x = playerWindow.y = 0;
                        }

                    }

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
            this.sendEvent(EditorEvents.IPCPlayerPauseResumeRequestEventType);
        }
    }

    invokeStepPausedPlayer() {
        if (Atomic.editorMode.isPlayerEnabled()) {
            this.sendEvent(EditorEvents.IPCPlayerPauseStepRequestEventType);
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
        this.sendEvent(Editor.EditorSaveResourceEventType);
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

        this.sendEvent(Editor.GizmoEditModeChangedEventData({ mode: mode }));

    }

    toggleGizmoAxisMode() {
        var editor = EditorUI.getCurrentResourceEditor();

        if (editor && editor instanceof Editor.SceneEditor3D) {
            var mode = editor.getGizmo().axisMode ? Editor.AxisMode.AXIS_WORLD : Editor.AxisMode.AXIS_LOCAL;
            this.sendEvent(Editor.GizmoAxisModeChangedEventData({ mode: mode }));
        }
    }

    invokeResourceFrameShortcut(shortcut: Editor.EditorShortcutType) {
        if (!ToolCore.toolSystem.project) return;
        var resourceFrame = EditorUI.getMainFrame().resourceframe.currentResourceEditor;
        if (resourceFrame) {
            resourceFrame.invokeShortcut(shortcut);
        }
    }

    invokeScreenshot() {
        var features = Preferences.getInstance().editorFeatures; // get prefs
        var pic_ext = features.screenshotFormat;
        var pic_path = features.screenshotPath;
        var dx = new Date();  // get the date NOW
        var datestring = dx.getFullYear() + "_" + ("0" + (dx.getMonth() + 1 )).slice(-2) + "_"  + ("0" + dx.getDate()).slice(-2)
            + "_" + ("0" + dx.getHours()).slice(-2) + "_" + ("0" + dx.getMinutes()).slice(-2) + "_" + ("0" + dx.getSeconds()).slice(-2);
        pic_path += "/Screenshot_" + datestring + "." + pic_ext;  // form filename
        var myimage = new Atomic.Image; // make an image to save
        if (Atomic.graphics.takeScreenShot(myimage)) { // take the screenshot
            var saved_pic = false;
            var jpgquality = 92; // very good quality jpeg 
            if ( pic_ext == "png" ) saved_pic = myimage.savePNG(pic_path);
            else if ( pic_ext == "jpg" ) saved_pic = myimage.saveJPG(pic_path, jpgquality);
            else if ( pic_ext == "tga" ) saved_pic = myimage.saveTGA(pic_path);
            else if ( pic_ext == "bmp" ) saved_pic = myimage.saveBMP(pic_path);
            else if ( pic_ext == "dds" ) saved_pic = myimage.saveDDS(pic_path);
            if (saved_pic)  EditorUI.showEditorStatus ( "Saved screenshot " + pic_path );
            else EditorUI.showEditorStatus ( "Error - could not save screenshot " + pic_path );
        }
        else EditorUI.showEditorStatus ( "Error - could not take screenshot.");
    }

    handleKeyDown(ev: Atomic.KeyDownEvent) {

        // if the right mouse buttons isn't down
        if (!(ev.buttons & Atomic.MOUSEB_RIGHT)) {

            // TODO: Make these customizable

            if (!Atomic.ui.focusedWidget && !this.cmdKeyDown()) {

                if (ev.key == Atomic.KEY_W) {
                    this.invokeGizmoEditModeChanged(Editor.EditMode.EDIT_MOVE);
                } else if (ev.key == Atomic.KEY_E) {
                    this.invokeGizmoEditModeChanged(Editor.EditMode.EDIT_ROTATE);
                } else if (ev.key == Atomic.KEY_R) {
                    this.invokeGizmoEditModeChanged(Editor.EditMode.EDIT_SCALE);
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

        if ( !cmdKey && ev.qualifiers > 0 ) { // check the event, the qualifier may have been programmitically set
            cmdKey = ( ev.qualifiers == Atomic.QUAL_CTRL );
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
            else if (ev.key == Atomic.KEY_9) {
                this.invokeScreenshot();
            }

        }

    }

}

export = Shortcuts;
