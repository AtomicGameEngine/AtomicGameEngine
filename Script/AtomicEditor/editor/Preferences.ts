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
        this.updateRecentProjects();
    }

    unRegisterRecentProject(path: string): void {
        var index = this._prefs.recentProjects.indexOf(path);
        if (index >= 0) {
            this._prefs.recentProjects.splice(index, 1);
        }
        this.updateRecentProjects();
    }

    updateRecentProjects(): void {
        for (var i in this._prefs.recentProjects) {
            var path = this._prefs.recentProjects[i];
            if (!this.fileSystem.exists(path)) {
                this._prefs.recentProjects.splice(i, 1);
            }
        }
    }

    deleteRecentProjects(): void {
        this._prefs.recentProjects.length = 0;
    }

    getPreferencesFullPath(): string {
        var filePath = this.fileSystem.getAppPreferencesDir("AtomicEditor", "Preferences");
        filePath += "prefs.json";
        return filePath;
    }

    read(): void {
        var filePath = this.getPreferencesFullPath();
        var jsonFile;
        //check if file doesn't exists, create an empty JSON file
        if (!this.fileSystem.fileExists(filePath)) {
            jsonFile = new Atomic.File(filePath, Atomic.FILE_WRITE);
            jsonFile.writeString("{}");
            jsonFile.close();
        }
        //Read file
        jsonFile = new Atomic.File(filePath, Atomic.FILE_READ);
        var prefs = <PreferencesFormat>JSON.parse(jsonFile.readText());
        if (prefs) {
            if (!prefs.recentProjects) prefs.recentProjects = [""];
            this._prefs = prefs;
        }

    }

    write(): void {
        var filePath = this.getPreferencesFullPath();
        var jsonFile = new Atomic.File(filePath, Atomic.FILE_WRITE);
        if (!jsonFile.isOpen()) return;
        var graphics = Atomic.getGraphics();
        var pos, width, height;
        if (graphics && !graphics.getFullscreen()) {
            pos = graphics.getWindowPosition();
            width = graphics.getWidth();
            height = graphics.getHeight();
        }
        this._prefs.window = { x: pos[0], y: pos[1], width: width, height: height };
        jsonFile.writeString(JSON.stringify(this._prefs, null, 2));
    }

    get recentProjects(): [string] {
        return this._prefs.recentProjects;
    }

    static getInstance(): Preferences {
        return Preferences.instance;
    }
}

class PreferencesFormat {
    recentProjects: [string];
    window: { x: number, y: number, width: number, height: number };
}

export = Preferences;
