
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
        var prefs = <PreferencesFormat> JSON.parse(jsonFile.readText());
        if (prefs) {
            if(!prefs.recentProjects) prefs.recentProjects = [""];
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

    get androidSDKPath(): string {
        return Atomic.addTrailingSlash(this._prefs.androidSDKPath);
    }

    set androidSDKPath(path: string) {
        this._prefs.androidSDKPath = path;
        this.write()
    }

    get jdkRootPath(): string {
        return Atomic.addTrailingSlash(this._prefs.jdkRootPath);
    }

    set jdkRootPath(path: string) {
        this._prefs.jdkRootPath = path;
        this.write();
    }

    get antPath(): string {
        return Atomic.addTrailingSlash(this._prefs.antPath);
    }

    set antPath(path: string) {
        this._prefs.antPath = path;
        this.write();
    }

    static getInstance(): Preferences {
        return Preferences.instance;
    }
}

class PreferencesFormat {
    recentProjects: [string];
    androidSDKPath: string;
    jdkRootPath: string;
    antPath: string;
    window: { x: number, y: number, width: number, height: number };
}

export = Preferences;
