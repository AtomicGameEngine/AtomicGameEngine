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

class Preferences {

    private static Ctor = (() => {
        new Preferences();
    })();

    private fileSystem: Atomic.FileSystem;

    private static instance: Preferences;
    private _prefs: PreferencesFormat;

    private cachedProjectPreferences: Object = null;

    constructor() {
        this.fileSystem = Atomic.getFileSystem();
        Preferences.instance = this;
    }

    registerRecentProject(path: string): void {
        var index = this._prefs.recentProjects.indexOf(path);
        if (index >= 0) {
            this._prefs.recentProjects.splice(index, 1);
        }
        this._prefs.recentProjects.unshift(path);
        this.updateRecentProjects(true);
    }

    updateRecentProjects(write: boolean = false): void {

        for (var i = 0; i < this._prefs.recentProjects.length; i++) {
            var path = this._prefs.recentProjects[i];
            if (!this.fileSystem.exists(path)) {
                this._prefs.recentProjects.splice(i, 1);
                write = true;
            }
        }
        if (write)
            this.write();
    }

    deleteRecentProjects(): void {
        this._prefs.recentProjects.length = 0;
        this.write();
    }

    getPreferencesFullPath(): string {
        var filePath = this.fileSystem.getAppPreferencesDir("AtomicEditor", "Preferences");
        filePath += "prefs.json";
        return filePath;
    }

    read(): void {
        var filePath = this.getPreferencesFullPath();
        var jsonFile;

        //check if file doesn't exist, create default json
        if (!this.fileSystem.fileExists(filePath)) {
            this.useDefaultConfig();
            this.write();
            return;
        }

        //Read file
        jsonFile = new Atomic.File(filePath, Atomic.FILE_READ);

        var prefs = null;

        try {

            if (jsonFile.isOpen())
                prefs = <PreferencesFormat>JSON.parse(jsonFile.readText());

        } catch (e) {

            prefs = null;
        }

        if (prefs && (!prefs.editorWindow || !prefs.playerWindow || !prefs.recentProjects)) {
            prefs = null;
        }

        if (prefs) {
            this._prefs = prefs;
        } else {
            console.log("Editor preference file invalid, regenerating default configuration");
            this.useDefaultConfig();
            this.write();
        }

    }

    write(): boolean {
        var filePath = this.getPreferencesFullPath();
        var jsonFile = new Atomic.File(filePath, Atomic.FILE_WRITE);
        if (!jsonFile.isOpen()) return false;
        jsonFile.writeString(JSON.stringify(this._prefs, null, 2));
    }

    saveEditorWindowData(windowData: WindowData) {
        this._prefs.editorWindow = windowData;
        this.write();
    }

    savePlayerWindowData(windowData: WindowData) {
        this._prefs.playerWindow = windowData;
        this.write();
    }

    useDefaultConfig(): void {
        this._prefs = new PreferencesFormat();
    }

    get editorWindow(): WindowData {
        return this._prefs.editorWindow;
    }

    get playerWindow(): WindowData {
        return this._prefs.playerWindow;
    }

    get recentProjects(): string[] {
        return this._prefs.recentProjects;
    }

    static getInstance(): Preferences {
        return Preferences.instance;
    }

    /**
     * Return a preference value or the provided default from the user settings file located in the project
     * @param  {string} settingsGroup name of the group these settings should fall under
     * @param  {string} preferenceName name of the preference to retrieve
     * @param  {number | boolean | string} defaultValue value to return if pref doesn't exist
     * @return {number|boolean|string}
     */
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: number): number;
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: string): string;
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: boolean): boolean;
    getUserPreference(settingsGroup: string, preferenceName: string, defaultValue?: any): any {

        // Cache the settings so we don't keep going out to the file
        if (this.cachedProjectPreferences == null) {
            const prefsFileLoc = ToolCore.toolSystem.project.userPrefsFullPath;
            if (Atomic.fileSystem.fileExists(prefsFileLoc)) {
                let prefsFile = new Atomic.File(prefsFileLoc, Atomic.FILE_READ);
                try {
                    let prefs = JSON.parse(prefsFile.readText());
                    this.cachedProjectPreferences = prefs;
                } finally {
                    prefsFile.close();
                }
            }
        }

        if (this.cachedProjectPreferences && this.cachedProjectPreferences[settingsGroup]) {
            return this.cachedProjectPreferences[settingsGroup][preferenceName] || defaultValue;
        }

        // if all else fails
        return defaultValue;
    }

    /**
     * Sets a user preference value in the user settings file located in the project
     * @param  {string} settingsGroup name of the group the preference lives under
     * @param  {string} preferenceName name of the preference to set
     * @param  {number | boolean | string} value value to set
     */
    setUserPreference(settingsGroup: string, preferenceName: string, value: number | boolean | string) {

        const prefsFileLoc = ToolCore.toolSystem.project.userPrefsFullPath;
        let prefs = {};

        if (Atomic.fileSystem.fileExists(prefsFileLoc)) {
            let prefsFile = new Atomic.File(prefsFileLoc, Atomic.FILE_READ);
            try {
                prefs = JSON.parse(prefsFile.readText());
            } finally {
                prefsFile.close();
            }
        }

        prefs[settingsGroup] = prefs[settingsGroup] || {};
        prefs[settingsGroup][preferenceName] = value;

        let saveFile = new Atomic.File(prefsFileLoc, Atomic.FILE_WRITE);
        try {
            saveFile.writeString(JSON.stringify(prefs, null, "  "));
        } finally {
            saveFile.flush();
            saveFile.close();
        }

        // Cache the update
        this.cachedProjectPreferences = prefs;
    }


    /**
     * Sets a group of user preference values in the user settings file located in the project.  Elements in the
     * group will merge in with existing group preferences.  Use this method if setting a bunch of settings
     * at once.
     * @param  {string} settingsGroup name of the group the preference lives under
     * @param  {string} groupPreferenceValues an object literal containing all of the preferences for the group.
     */
    setUserPreferenceGroup(settingsGroup: string, groupPreferenceValues: Object) {

        const prefsFileLoc = ToolCore.toolSystem.project.userPrefsFullPath;
        let prefs = {};

        if (Atomic.fileSystem.fileExists(prefsFileLoc)) {
            let prefsFile = new Atomic.File(prefsFileLoc, Atomic.FILE_READ);
            try {
                prefs = JSON.parse(prefsFile.readText());
            } finally {
                prefsFile.close();
            }
        }

        prefs[settingsGroup] = prefs[settingsGroup] || {};
        for (let preferenceName in groupPreferenceValues) {
            prefs[settingsGroup][preferenceName] = groupPreferenceValues[preferenceName];
        }

        let saveFile = new Atomic.File(prefsFileLoc, Atomic.FILE_WRITE);
        try {
            saveFile.writeString(JSON.stringify(prefs, null, "  "));
        } finally {
            saveFile.flush();
            saveFile.close();
        }

        // Cache the update
        this.cachedProjectPreferences = prefs;
    }
}

interface WindowData {
    x: number;
    y: number;
    width: number;
    height: number;
    monitor: number;
    maximized: boolean;
}

class PreferencesFormat {

    constructor() {

        this.setDefault();
    }

    setDefault() {

        this.recentProjects = [];

        this.editorWindow = {
            x: 0,
            y: 0,
            width: 0,
            height: 0,
            monitor: 0,
            maximized: true
        };

        this.playerWindow = {
            x: 0,
            y: 0,
            width: 0,
            height: 0,
            monitor: 0,
            maximized: false
        };

    }

    recentProjects: string[];
    editorWindow: WindowData;
    playerWindow: WindowData;
}

export = Preferences;
