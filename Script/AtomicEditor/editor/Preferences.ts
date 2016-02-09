//
// Copyright (c) 2014-2015, THUNDERBEAST GAMES LLC All rights reserved
// LICENSE: Atomic Game Engine Editor and Tools EULA
// Please see LICENSE_ATOMIC_EDITOR_AND_TOOLS.md in repository root for
// license information: https://github.com/AtomicGameEngine/AtomicGameEngine
//

class Preferences {

    private static Ctor = (() => {
        new Preferences();
    })();

    private fileSystem: Atomic.FileSystem;

    private static instance: Preferences;
    private _prefs: PreferencesFormat;

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
