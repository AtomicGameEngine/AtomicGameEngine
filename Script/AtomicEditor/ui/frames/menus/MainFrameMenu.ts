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

import strings = require("../../EditorStrings");
import EditorUI = require("../../EditorUI");
import MenuItemSources = require("./MenuItemSources");
import Preferences = require("editor/Preferences");
import ServiceLocator from "../../../hostExtensions/ServiceLocator";

class MainFrameMenu extends Atomic.ScriptObject {

    private pluginMenuItemSource: Atomic.UIMenuItemSource;

    constructor() {

        super();

        MenuItemSources.createMenuItemSource("menu edit", editItems);
        MenuItemSources.createMenuItemSource("menu file", fileItems);
        MenuItemSources.createMenuItemSource("menu build", buildItems);
        MenuItemSources.createMenuItemSource("menu tools", toolsItems);
        MenuItemSources.createMenuItemSource("menu developer", developerItems);
        MenuItemSources.createMenuItemSource("menu help", helpItems);
        this.goScreenshot = 0;
    }

    createPluginMenuItemSource(id: string, items: any): Atomic.UIMenuItemSource {
        if (!this.pluginMenuItemSource) {
            var developerMenuItemSource = MenuItemSources.getMenuItemSource("menu developer");
            this.pluginMenuItemSource = MenuItemSources.createSubMenuItemSource(developerMenuItemSource, "Plugins", {});
        }

        return MenuItemSources.createSubMenuItemSource(this.pluginMenuItemSource , id, items);

    }

    removePluginMenuItemSource(id: string) {
        if (this.pluginMenuItemSource) {
            this.pluginMenuItemSource.removeItemWithStr(id);
            if (0 == this.pluginMenuItemSource.itemCount) {
                var developerMenuItemSource = MenuItemSources.getMenuItemSource("menu developer");
                developerMenuItemSource.removeItemWithStr("Plugins");
                this.pluginMenuItemSource = null;
            }
        }
    }

    handleScreenshot(ev) {
        if ( this.goScreenshot > 0 ) {
            this.goScreenshot--;
            if ( this.goScreenshot == 0 ) {
                EditorUI.getShortcuts().invokeScreenshot();
                this.unsubscribeFromEvent("Update");
            }
        }
    }

    handlePopupMenu(target: Atomic.UIWidget, refid: string): boolean {

        if (target.id == "menu edit popup") {

            if (refid == "edit play") {
                EditorUI.getShortcuts().invokePlayOrStopPlayer();
                return true;
            }

            if (refid == "edit pause") {
                EditorUI.getShortcuts().invokePauseOrResumePlayer();
                return true;
            }

            if (refid == "edit step") {
                EditorUI.getShortcuts().invokeStepPausedPlayer();
                return true;
            }

            if (refid == "edit play debug") {
                EditorUI.getShortcuts().invokePlayOrStopPlayer(true);
                return true;
            }

            if (refid == "edit format code") {
                EditorUI.getShortcuts().invokeFormatCode();
                return true;
            }

            if (refid == "edit undo") {
                EditorUI.getShortcuts().invokeUndo();
                return true;
            }

            if (refid == "edit redo") {
                EditorUI.getShortcuts().invokeRedo();
                return true;
            }

            if (refid == "edit cut") {
                EditorUI.getShortcuts().invokeCut();
                return true;
            }

            if (refid == "edit copy") {
                EditorUI.getShortcuts().invokeCopy();
                return true;
            }

            if (refid == "edit paste") {
                EditorUI.getShortcuts().invokePaste();
                return true;
            }

            if (refid == "edit select all") {
                EditorUI.getShortcuts().invokeSelectAll();
                return true;
            }

            if (refid == "edit snap settings") {
                EditorUI.getModelOps().showSnapSettings();
                return true;
            }

            if (refid == "edit frame selected") {
                EditorUI.getShortcuts().invokeFrameSelected();
                return true;
            }


            return false;

        } else if (target.id == "menu file popup") {
            if (refid == "quit") {

                this.sendEvent(Atomic.ExitRequestedEventType);
                return true;

            }
            if (refid == "file new project") {

                if (ToolCore.toolSystem.project) {

                    EditorUI.showModalError("Project Open",
                        "Please close the current project before creating a new one");

                    return true;
                }

                var mo = EditorUI.getModelOps();
                mo.showNewProject();

                return true;

            }
            if (refid == "file open project") {

                var utils = new Editor.FileUtils();
                var path = utils.openProjectFileDialog();

                if (path == "") {

                    return true;

                }

                var requestProjectLoad = () => this.sendEvent(Editor.RequestProjectLoadEventData({ path: path }));

                if (ToolCore.toolSystem.project) {

                    this.subscribeToEvent(Editor.EditorProjectClosedEvent(() => {

                        this.unsubscribeFromEvent(Editor.EditorProjectClosedEventType);
                        requestProjectLoad();

                    }));

                    this.sendEvent(Editor.EditorCloseProjectEventType);

                } else {

                    requestProjectLoad();

                }

                return true;

            }

            if (refid == "file close project") {

                this.sendEvent(Editor.EditorCloseProjectEventType);

                return true;

            }

            if (refid == "file save file") {
                EditorUI.getShortcuts().invokeFileSave();
                return true;
            }

            if (refid == "file close file") {
                EditorUI.getShortcuts().invokeFileClose();
                return true;
            }

            if (refid == "file save all") {
                this.sendEvent(Editor.EditorSaveAllResourcesEventType);
                return true;
            }

            return false;

        } else if (target.id == "menu developer popup") {

            if (refid == "toggle theme") {
                Preferences.getInstance().toggleTheme();
                return true;
            }

            if (refid == "toggle codeeditor") {
                var ctheme = EditorUI.getEditor().getApplicationPreference( "codeEditor", "theme", "");
                if ( ctheme == "vs-dark" )
                    EditorUI.getEditor().setApplicationPreference( "codeEditor", "theme", "vs");
                else
                    EditorUI.getEditor().setApplicationPreference( "codeEditor", "theme", "vs-dark");
                return true;
            }

            if ( refid == "screenshot") {
                this.subscribeToEvent(Atomic.UpdateEvent((ev) => this.handleScreenshot(ev)));
                this.goScreenshot = 19;  // number of ticks to wait for the menu to close
                return true;
            }

            if (refid == "developer show console") {
                Atomic.ui.showConsole(true);
                return true;
            }

            if (refid == "developer show uidebugger") {

                if (Atomic.engine.debugBuild) {
                    Atomic.UI.debugShowSettingsWindow(EditorUI.getView());
                }
                else {
                    EditorUI.showModalError("Debug Build Required",
                        "UIDebugger currently requires a Debug engine build");
                }

                return true;
            }

            if (refid == "developer assetdatabase scan") {

              ToolCore.assetDatabase.scan();
              return true;

            }

            if (refid == "developer assetdatabase force") {

              ToolCore.assetDatabase.reimportAllAssets();
              return true;

            }

            //Sets all value in prefs.json to default and shuts down the editor.
            if (refid == "developer clear preferences") {
                var myPrefs = Preferences.getInstance();
                myPrefs.useDefaultConfig();
                myPrefs.saveEditorWindowData(myPrefs.editorWindow);
                myPrefs.savePlayerWindowData(myPrefs.playerWindow);
                Atomic.getEngine().exit();
                return true;
            }

            // If we got here, then we may have been injected by a plugin.  Notify the plugins
            return ServiceLocator.uiServices.menuItemClicked(refid);

        } else if (target.id == "menu tools popup") {

            if (refid == "tools toggle profiler") {
                Atomic.ui.toggleDebugHud();
                return true;
            } if (refid == "tools perf profiler") {
                Atomic.ui.debugHudProfileMode = Atomic.DebugHudProfileMode.DEBUG_HUD_PROFILE_PERFORMANCE;
                Atomic.ui.showDebugHud(true);
                return true;
            } else if (refid == "tools metrics profiler") {
                Atomic.ui.debugHudProfileMode = Atomic.DebugHudProfileMode.DEBUG_HUD_PROFILE_METRICS;
                Atomic.ui.showDebugHud(true);
                return true;
            } else if (refid.indexOf("tools log") != -1) {

                let logName = refid.indexOf("editor") != -1 ? "AtomicEditor" : "AtomicPlayer";
                let logFolder = Atomic.fileSystem.getAppPreferencesDir(logName, "Logs");
                Atomic.fileSystem.systemOpen(logFolder);
            }

        } else if (target.id == "menu build popup") {

            if (refid == "build build") {

                EditorUI.getModelOps().showBuild();

                return true;

            } else if (refid == "build settings") {

                EditorUI.getModelOps().showBuildSettings();
                return true;

            }

        } else if (target.id == "menu help popup") {

            if (refid == "about atomic editor") {
                EditorUI.getModelOps().showAbout();
                return true;
            }

            if (refid == "help what new") {
                EditorUI.getModelOps().showNewBuildWindow(false);
                return true;
            }

            let urlLookup = {

                "help doc wiki" : "https://github.com/AtomicGameEngine/AtomicGameEngine/wiki/",
                "help chat" : "https://gitter.im/AtomicGameEngine/AtomicGameEngine/",
                "help api js" : "http://docs.atomicgameengine.com/api/modules/atomic.html",
                "help api csharp" : "http://docs.atomicgameengine.com/csharp/AtomicEngine/",
                "help api cplusplus" : "http://docs.atomicgameengine.com/cpp",
                "help support" : "https://discourse.atomicgameengine.com/",
                "help github" : "https://github.com/AtomicGameEngine/AtomicGameEngine/"
            };

            if (urlLookup[refid]) {
                Atomic.fileSystem.systemOpen(urlLookup[refid]);
                return true;
            }

            return false;

        } else {
            // console.log("Menu: " + target.id + " clicked");
        }

    }

    goScreenshot: number;

}

export = MainFrameMenu;

// initialization
var StringID = strings.StringID;

var editItems = {

    "Undo": ["edit undo", StringID.ShortcutUndo],
    "Redo": ["edit redo", StringID.ShortcutRedo],
    "-1": null,
    "Cut": ["edit cut", StringID.ShortcutCut],
    "Copy": ["edit copy", StringID.ShortcutCopy],
    "Paste": ["edit paste", StringID.ShortcutPaste],
    "Select All": ["edit select all", StringID.ShortcutSelectAll],
    "-2": null,
    "Frame Selected": ["edit frame selected", StringID.ShortcutFrameSelected],
    "-3": null,
    "Find": ["edit find", StringID.ShortcutFind],
    "Find Next": ["edit find next", StringID.ShortcutFindNext],
    "Find Prev": ["edit find prev", StringID.ShortcutFindPrev],
    "-4": null,
    "Format Code": ["edit format code", StringID.ShortcutBeautify],
    "-5": null,
    "Play": ["edit play", StringID.ShortcutPlay],
    "Pause/Resume": ["edit pause", StringID.ShortcutPause],
    "Step": ["edit step", StringID.ShortcutStep],
    "Debug (C# Project)": ["edit play debug", StringID.ShortcutPlayDebug],
    "-6": null,
    "Snap Settings": ["edit snap settings"]

};

var toolsItems = {
    "Profiler": {
        "Toggle HUD": ["tools toggle profiler"],
        "Profile Performance": ["tools perf profiler"],
        "Profile Metrics": ["tools metrics profiler"]
    },
    "Logs": {
        "Player Log": ["tools log player"],
        "Editor Log": ["tools log editor"]
    }


};

var buildItems = {

    "Build": ["build build", StringID.ShortcutBuild],
    "Build Settings": ["build settings", StringID.ShortcutBuildSettings]
};


var developerItems = {

    "Toggle Theme": ["toggle theme"],
    "Toggle Code Editor Theme": ["toggle codeeditor"],
    "ScreenShot": ["screenshot", StringID.ShortcutScreenshot],
    "Show Console": ["developer show console"],
    "Clear Preferences": ["developer clear preferences"], //Adds clear preference to developer menu items list
    "Debug": {
        "UI Debugger": ["developer show uidebugger"],
        "Asset Database": {
            "Scan": ["developer assetdatabase scan"],
            "Force Reimport": ["developer assetdatabase force"]
        }
    }

};

var fileItems = {

    "New Project": ["file new project"],
    "Open Project": ["file open project"],
    "Save Project": ["file save project"],
    "-1": null,
    "Close Project": ["file close project"],
    "-2": null,
    "Save File": ["file save file", StringID.ShortcutSaveFile],
    "Save All Files": ["file save all"],
    "Close File": ["file close file", StringID.ShortcutCloseFile],
    "-3": null,
    "Quit": "quit"
};

var helpItems = {

    "Atomic Community Support": ["help support"],
    "Atomic Chat": ["help chat"],
    "-1": null,
    "Documentation Wiki": "help doc wiki",
    "API References": {
        "JavaScript & TypeScript": ["help api js"],
        "C#": ["help api csharp"],
        "C++": ["help api cplusplus"]
    },
    "-2": null,
    "Atomic Game Engine on GitHub": ["help github"],
    "-3": null,
    "What's New": "help what new",
    "About Atomic Editor": "about atomic editor"
};
