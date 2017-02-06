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

import EditorUI = require("ui/EditorUI");
import PlayMode = require("ui/playmode/PlayMode");
import EditorLicense = require("./EditorLicense");
import EditorEvents = require("./EditorEvents");
import Preferences = require("./Preferences");

class AtomicEditor extends Atomic.ScriptObject {

    project: ToolCore.Project;
    editorLicense: EditorLicense;
    playMode: PlayMode;

    static instance: AtomicEditor;

    projectCloseRequested: boolean;
    exitRequested: boolean;

    constructor() {

        super();

        // limit the framerate to limit CPU usage
        Atomic.getEngine().maxFps = 60;

        Atomic.getEngine().autoExit = false;

        AtomicEditor.instance = this;

        Preferences.getInstance().read();

        this.initUI();

        this.editorLicense = new EditorLicense();

        EditorUI.initialize(this);

        this.playMode = new PlayMode();

        Atomic.getResourceCache().autoReloadResources = true;

        this.subscribeToEvent(Editor.EditorLoadProjectEvent((data) => this.handleEditorLoadProject(data)));
        this.subscribeToEvent(Editor.EditorCloseProjectEvent((data) => this.handleEditorCloseProject(data)));
        this.subscribeToEvent(Editor.ProjectUnloadedNotificationEvent((data) => {
            Atomic.graphics.windowTitle = "AtomicEditor";
            this.handleProjectUnloaded(data);
        }));

        this.subscribeToEvent(Atomic.ScriptEvent(EditorEvents.IPCPlayerWindowChangedEventType, (data: AtomicApp.IPCPlayerWindowChangedEvent) => {
            var playerWindow = Preferences.getInstance().playerWindow;
            //if player window is maximized, then we want keep the window size from the previous state
            if (data.maximized) {
                playerWindow.x = data.posX;
                playerWindow.y = data.posY;
                playerWindow.monitor = data.monitor;
                playerWindow.maximized = true;
            } else {
                playerWindow = {x: data.posX, y: data.posY, width: data.width, height: data.height, monitor: data.monitor, maximized: data.maximized};
            }
            Preferences.getInstance().savePlayerWindowData(playerWindow);
        }));

        this.subscribeToEvent(Atomic.ScreenModeEvent((data) => this.saveWindowPreferences()));
        this.subscribeToEvent(Atomic.WindowPosEvent((data) => this.saveWindowPreferences()));

        this.subscribeToEvent(Atomic.ExitRequestedEvent((data) => this.handleExitRequested(data)));

        this.subscribeToEvent(ToolCore.ProjectLoadedEvent((data) => {
            Atomic.graphics.windowTitle = "AtomicEditor - " + data.projectPath;
            Preferences.getInstance().registerRecentProject(data.projectPath);
        }));

        this.subscribeToEvent(Editor.EditorResourceCloseCanceledEvent((data) => {
            //if user canceled closing the resource, then user has changes that he doesn't want to lose
            //so cancel exit/project close request and unsubscribe from event to avoid closing all the editors again
            this.exitRequested = false;
            this.projectCloseRequested = false;
            this.unsubscribeFromEvent(Editor.EditorResourceCloseEventType);
        }));

        this.parseArguments();
    }

    initUI() {
        var uiData = Preferences.getInstance().uiData;
        var ui = Atomic.ui;
        ui.loadSkin(uiData.skinPath + "/skin.tb.txt", uiData.defaultSkinPath + "/skin.tb.txt");
        ui.addFont(uiData.fontFile, uiData.fontName);
        ui.addFont("AtomicEditor/resources/MesloLGS-Regular.ttf", "Monaco");
        ui.setDefaultFont(uiData.fontName, uiData.fontSize);
    }

    saveWindowPreferences(): boolean {
        var graphics = Atomic.getGraphics();
        if (!graphics) return false;

        var pos = graphics.getWindowPosition();
        var width = graphics.getWidth();
        var height = graphics.getHeight();
        var monitor = graphics.getCurrentMonitor();

        var editorWindowData = Preferences.getInstance().editorWindow;

        if (graphics.getMaximized()) {
            editorWindowData.x = pos[0];
            editorWindowData.y = pos[1];
            editorWindowData.maximized = true;
            editorWindowData.monitor = monitor;
        } else {
            editorWindowData = {x: pos[0], y: pos[1], width: width, height: height, monitor: monitor, maximized: false};
        }

        Preferences.getInstance().saveEditorWindowData(editorWindowData);

        return true;
    }

    /**
     * Return a preference value or the provided default from the user settings file
     * @param  {string} extensionName name of the extension the preference lives under
     * @param  {string} preferenceName name of the preference to retrieve
     * @param  {number | boolean | string} defaultValue value to return if pref doesn't exist
     * @return {number|boolean|string}
     */
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: number): number;
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: string): string;
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: boolean): boolean;
    getUserPreference(extensionName: string, preferenceName: string, defaultValue?: any): any {
        return Preferences.getInstance().getUserPreference(extensionName, preferenceName, defaultValue);
    }


    /**
     * Sets a user preference value in the project user settings file
     * @param  {string} extensionName name of the extension the preference lives under
     * @param  {string} preferenceName name of the preference to set
     * @param  {number | boolean | string} value value to set
     */
    setUserPreference(extensionName: string, preferenceName: string, value: number | boolean | string) {
        Preferences.getInstance().setUserPreference(extensionName, preferenceName, value);
        WebView.WebBrowserHost.setGlobalStringProperty("HOST_Preferences", "ProjectPreferences", JSON.stringify(Preferences.getInstance().cachedProjectPreferences, null, 2 ));
        const eventData: Editor.UserPreferencesChangedNotificationEvent = {
            projectPreferences: JSON.stringify(Preferences.getInstance().cachedProjectPreferences),
            applicationPreferences: JSON.stringify(Preferences.getInstance().cachedApplicationPreferences)
        };

        this.sendEvent(Editor.UserPreferencesChangedNotificationEventData(eventData));
    }

    /**
     * Return a preference value or the provided default from the global user settings file
     * @param  {string} groupName name of the section the preference lives under
     * @param  {string} preferenceName name of the preference to retrieve
     * @param  {number | boolean | string} defaultValue value to return if pref doesn't exist
     * @return {number|boolean|string}
     */
    getApplicationPreference(settingsGroup: string, preferenceName: string, defaultValue?: number): number;
    getApplicationPreference(settingsGroup: string, preferenceName: string, defaultValue?: string): string;
    getApplicationPreference(settingsGroup: string, preferenceName: string, defaultValue?: boolean): boolean;
    getApplicationPreference(groupName: string, preferenceName: string, defaultValue?: any): any {
        return Preferences.getInstance().getApplicationPreference(groupName, preferenceName, defaultValue);
    }

    /**
     * Sets a user preference value in the global application settings file
     * @param  {string} groupName name of the section the preference lives under
     * @param  {string} preferenceName name of the preference to set
     * @param  {number | boolean | string} value value to set
     */
    setApplicationPreference(groupName: string, preferenceName: string, value: number | boolean | string) {
        Preferences.getInstance().setApplicationPreference(groupName, preferenceName, value);
        WebView.WebBrowserHost.setGlobalStringProperty("HOST_Preferences", "ApplicationPreferences", JSON.stringify(Preferences.getInstance().cachedApplicationPreferences, null, 2 ));
        const eventData: Editor.UserPreferencesChangedNotificationEvent = {
            projectPreferences: JSON.stringify(Preferences.getInstance().cachedProjectPreferences),
            applicationPreferences: JSON.stringify(Preferences.getInstance().cachedApplicationPreferences)
        };

        this.sendEvent(Editor.UserPreferencesChangedNotificationEventData(eventData));
    }

    /**
     * Sets a group of user preference values in the user settings file located in the project.  Elements in the
     * group will merge in with existing group preferences.  Use this method if setting a bunch of settings
     * at once.
     * @param  {string} settingsGroup name of the group the preference lives under
     * @param  {string} groupPreferenceValues an object literal containing all of the preferences for the group.
     */
    setUserPreferenceGroup(settingsGroup: string, groupPreferenceValues: Object) {
        Preferences.getInstance().setUserPreferenceGroup(settingsGroup, groupPreferenceValues);
        WebView.WebBrowserHost.setGlobalStringProperty("HOST_Preferences", "ProjectPreferences", JSON.stringify(Preferences.getInstance().cachedProjectPreferences, null, 2 ));
        const eventData: Editor.UserPreferencesChangedNotificationEvent = {
            projectPreferences: JSON.stringify(Preferences.getInstance().cachedProjectPreferences),
            applicationPreferences: JSON.stringify(Preferences.getInstance().cachedApplicationPreferences)
        };

        this.sendEvent(Editor.UserPreferencesChangedNotificationEventData(eventData));
    }

    handleEditorLoadProject(event: Editor.LoadProjectNotificationEvent): boolean {

        var system = ToolCore.getToolSystem();
        if (system.project) {

            this.sendEvent(Editor.EditorModalEventData({ type: Editor.EDITOR_MODALERROR, title: "Project already loaded", message: "Project already loaded" }));

            return false;

        }
        const loaded = system.loadProject(event.path);
        if (loaded) {
            Preferences.getInstance().loadUserPrefs();
            WebView.WebBrowserHost.setGlobalStringProperty("HOST_Preferences", "ProjectPreferences", JSON.stringify(Preferences.getInstance().cachedProjectPreferences, null, 2 ));
            WebView.WebBrowserHost.setGlobalStringProperty("HOST_Preferences", "ApplicationPreferences", JSON.stringify(Preferences.getInstance().cachedApplicationPreferences, null, 2 ));
            this.sendEvent(Editor.LoadProjectNotificationEventData(event));
        }
        return loaded;
    }

    closeAllResourceEditors() {
        var editor = EditorUI.getCurrentResourceEditor();
        if (!editor) {
          if (this.exitRequested) {
              this.exit();
          } else if (this.projectCloseRequested) {
              this.closeProject();
          }
          return;
        }
        //wait when we close resource editor to check another resource editor for unsaved changes and close it
        this.subscribeToEvent(Editor.EditorResourceCloseEvent((data) => {
            this.closeAllResourceEditors();
        }));
        editor.requestClose();
    }

    handleEditorCloseProject(event: Editor.ProjectUnloadedNotificationEvent) {
        this.projectCloseRequested = true;
        this.sendEvent(Editor.ProjectUnloadedNotificationEventData());
        this.closeAllResourceEditors();
    }

    closeProject() {
        this.sendEvent(EditorEvents.IPCPlayerExitRequestEventType);
        var system = ToolCore.getToolSystem();

        if (system.project) {

            system.closeProject();
            this.sendEvent(Editor.EditorProjectClosedEventType);
            this.projectCloseRequested = false;
            this.unsubscribeFromEvent(Editor.EditorResourceCloseEventType);
        }

    }

    handleProjectUnloaded(event) {

        this.sendEvent(Editor.EditorActiveSceneEditorChangeEventData({ sceneEditor: null }));

    }

    parseArguments() {

        var args = Atomic.getArguments();

        var idx = 0;

        while (idx < args.length) {

            if (args[idx] == "--project") {

                this.sendEvent(Editor.EditorLoadProjectEventData({ path: args[idx + 1] }));

            }

            idx++;

        }

    }

    // event handling
    handleExitRequested(data) {
        if (this.exitRequested) return;
        this.exitRequested = true;
        this.closeAllResourceEditors();
    }

    exit() {
        //Preferences.getInstance().write();
        EditorUI.shutdown();
        Atomic.getEngine().exit();
    }


}

export default AtomicEditor;
