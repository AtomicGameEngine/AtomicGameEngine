
class Preferences {

    private fileSystem: Atomic.FileSystem;

    private static instance: Preferences;
    private _androidSDKPath: string;
    private _jdkRootPath: string;
    private _antPath: string;

    private _recentProjects: [string];

    constructor() {
        this.fileSystem = Atomic.getFileSystem();
        Preferences.instance = this;

        this._recentProjects = [""];
    }

    registerRecentProject(path: string): void {
        var index = this._recentProjects.indexOf(path);
        if (index >= 0) {
            this._recentProjects.splice(index, 1);
        }
        this._recentProjects.unshift(path);
        this.updateRecentProjects();
    }

    unRegisterRecentProject(path: string): void {
        var index = this._recentProjects.indexOf(path);
        if (index >= 0) {
            this._recentProjects.splice(index, 1);
        }
        this.updateRecentProjects();
    }

    updateRecentProjects(): void {
        for (var i in this._recentProjects) {
            var path = this._recentProjects[i];
            if (!this.fileSystem.exists(path)) {
                this._recentProjects.splice(i, 1);
            }
        }
    }

    deleteRecentProjects(): void {
        this._recentProjects.length = 0;
    }

    getPreferencesFullPath(): string {
        var filePath = this.fileSystem.getAppPreferencesDir("AtomicEditor", "Preferences");
        filePath += "prefs.json";
        return filePath;
    }

    read(): void {
        var filePath = this.getPreferencesFullPath();
        var jsonFile = new Atomic.File(filePath, Atomic.FILE_READ);
        if (!jsonFile.isOpen()) return;
        var prefs = <PreferencesFormat> JSON.parse(jsonFile.readText());
        console.log(prefs);
        this._recentProjects = prefs.recentFiles;
        this._androidSDKPath = prefs.androidSdkPath;
        this._jdkRootPath = prefs.jdkRootPath;
        this._antPath = prefs.antPath;
    }

    write(): void {
        var filePath = this.getPreferencesFullPath();
        var jsonFile = new Atomic.File(filePath, Atomic.FILE_WRITE);
        if (!jsonFile.isOpen()) return;
        var prefs = new PreferencesFormat();
        prefs.recentFiles = this._recentProjects;
        var graphics = Atomic.getGraphics();
        var pos, width, height;
        if (graphics && !graphics.getFullscreen()) {
            pos = graphics.getWindowPosition();
            width = graphics.getWidth();
            height = graphics.getHeight();
        }
        prefs.window = { x: pos[0], y: pos[1], width: width, height: height };
        prefs.androidSdkPath = this._androidSDKPath;
        prefs.jdkRootPath = this._jdkRootPath;
        prefs.antPath = this._antPath;
        jsonFile.writeString(JSON.stringify(prefs, null, 2));
    }

    get recentProjects(): [string] {
        return this._recentProjects;
    }

    get androidSdkPath(): string {
        return Atomic.addTrailingSlash(this._androidSDKPath);
    }

    set androidSdkPath(path: string) {
        this._androidSDKPath = path;
        this.write()
    }

    get jdkRootPath(): string {
        return Atomic.addTrailingSlash(this._jdkRootPath);
    }

    set jdkRootPath(path: string) {
        this._jdkRootPath = path;
        this.write();
    }

    get antPath(): string {
        return Atomic.addTrailingSlash(this._antPath);
    }

    set andPath(path: string) {
        this._antPath = path;
        this.write();
    }

    static getInstance(): Preferences {
        return Preferences.instance;
    }
}

class PreferencesFormat {
    recentFiles: [string];
    androidSdkPath: string;
    jdkRootPath: string;
    antPath: string;
    window: { x: number, y: number, width: number, height: number };
}

export = Preferences;