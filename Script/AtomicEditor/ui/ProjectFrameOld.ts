

import ScriptWidget = require("./ScriptWidget");
import Editor = require("../editor/Editor");

class ProjectFrame extends ScriptWidget {

    folderList: Atomic.UIListView;

    constructor(parent: Atomic.UIWidget) {

        super();

        this.load("AtomicEditor/editor/ui/projectframe.tb.txt");

        this.gravity = Atomic.UI_GRAVITY_TOP_BOTTOM;

        var projectviewcontainer = parent.getWidget("projectviewcontainer");

        projectviewcontainer.addChild(this);

        var foldercontainer = this.getWidget("foldercontainer");

        var folderList = this.folderList = new Atomic.UIListView();

        folderList.rootList.id = "folderList_";

        foldercontainer.addChild(folderList);

        // events
        this.subscribeToEvent("ProjectLoaded", (data) => this.handleProjectLoaded(data));

    }


    handleProjectLoaded(data) {

        this.refresh();

    }

    handleWidgetEvent(data): boolean {

        if (data.type == Atomic.UI_EVENT_TYPE_CLICK) {

            var fs = Atomic.getFileSystem();

            if (data.target && data.target.id.length) {

                var id = data.target.id;

                if (id == "folderList_") {

                    var list = <Atomic.UISelectList> data.target;

                    var selectedId = list.selectedItemID;

                    if (fs.dirExists(selectedId)) {
                        this.selectCurrentContentFolder(selectedId);
                    }

                    return true;

                }

                if (id == "..") {
                    var parentPath = Atomic.getParentPath(this.currentContentFolder);
                    this.selectCurrentContentFolder(parentPath);
                    return true;
                }

                if (fs.dirExists(id)) {
                    this.selectCurrentContentFolder(id);
                    return true;
                }

                if (fs.fileExists(id)) {
                    this.sendEvent("EditResource", { "path": id });
                    return true;
                }

            }

        }

        return false;

    }


    refresh() {

        var cfolder = this.currentContentFolder;
        this.currentContentFolder = "";
        this.refreshFolders();

    }

    private refreshContent(fullpath: string) {

        var system = ToolCore.getToolSystem();
        var project = system.project;

        this.currentContentFolder = fullpath;

        var folders = new Array<string>();
        var content = new Array<string>();

        this.scanContentDirForContent(folders, content, fullpath);

        var container: Atomic.UILayout = <Atomic.UILayout> this.getWidget("contentcontainer");

        container.deleteAllChildren();

        if (fullpath != project.resourcePath) {

            var lp = new Atomic.UILayoutParams();
            lp.height = 20;

            var fd = new Atomic.UIFontDescription();
            fd.id = "Vera";
            fd.size = 11;

            var button = new Atomic.UIButton();
            button.gravity = Atomic.UI_GRAVITY_LEFT;
            button.text = "..                     ";
            button.id = "..";
            button.skinBg = "TBButton.flat";
            button.layoutParams = lp;
            button.fontDescription = fd;

            container.addChild(button);

        }

        for (var folder of folders) {

            var contentID = Atomic.addTrailingSlash(fullpath + folder);
            container.addChild(this.createButtonLayout(contentID, folder));
        }

        for (var c of content) {

            var contentID = Atomic.addTrailingSlash(fullpath) + c;
            container.addChild(this.createButtonLayout(contentID, c));
        }


    }

    private recursiveAddFolder(parentItemID: number, fullpath: string, folderName: string) {

        var folderList = this.folderList;

        var childItemID = folderList.addChildItem(parentItemID, folderName, "Folder.icon", fullpath);

        var dirs = new Array<string>();

        this.scanDirForFolders(dirs, fullpath);

        for (var folder of dirs) {
            this.recursiveAddFolder(childItemID, fullpath + folder + "/", folder);
        }

    }

    private refreshFolders() {

        var system = ToolCore.getToolSystem();
        var project = system.project;

        var folderList = this.folderList;

        folderList.deleteAllItems();

        var resourcesID = folderList.addRootItem("Resources", "Folder.icon", project.resourcePath);

        var dirs = new Array<string>();

        this.scanDirForFolders(dirs, project.resourcePath);

        for (var folder of dirs) {
            this.recursiveAddFolder(resourcesID, project.resourcePath + folder + "/", folder);
        }

        folderList.setExpanded(resourcesID, true);
        this.refreshContent(project.resourcePath);
        folderList.rootList.value = 0;


    }

    private selectCurrentContentFolder(folder: string) {

        this.folderList.selectItemByID(folder);
        if (this.currentContentFolder != folder)
            this.refreshContent(folder);
    }

    private endsWith(str: string, suffix: string): boolean {
        return str && str.indexOf(suffix, str.length - suffix.length) !== -1;
    }

    private scanDirForFolders(dirs: Array<string>, fullpath: string) {

        var fileSystem = Atomic.getFileSystem();

        dirs.length = 0;

        var folders = fileSystem.scanDir(fullpath, "", Atomic.SCAN_DIRS, false);

        for (var folder of folders) {

            if (folder == "." || folder == ".." || this.endsWith(folder, "/..") || this.endsWith(folder, "/."))
                continue;

            dirs.push(folder);

        }

    }


    private scanContentDirForContent(folders: Array<string>, content: Array<string>, fullPath: string) {

        var fileSystem = Atomic.getFileSystem();

        folders.length = content.length = 0;

        var _folders = fileSystem.scanDir(fullPath, "", Atomic.SCAN_DIRS, false);

        for (var folder of _folders) {

            if (folder == "." || folder == ".." || this.endsWith(folder, "/..") || this.endsWith(folder, "/."))
                continue;

            folders.push(folder);

        }

        var _content = fileSystem.scanDir(fullPath, "", Atomic.SCAN_FILES, false);

        for (var c of _content) {

            content.push(c);

        }

    }

    private createButtonLayout(fullpath: string, text: string): Atomic.UILayout {

        var system = ToolCore.getToolSystem();
        var project = system.project;
        var fs = Atomic.getFileSystem();

        var pathinfo = Atomic.splitPath(fullpath);

        var bitmapID = "Folder.icon";

        if (fs.fileExists(fullpath)) {
            bitmapID = "FileBitmap";
        }

        if (pathinfo.ext == ".js") {
            if (project.isComponentsDirOrFile(fullpath)) {
                bitmapID = "ComponentBitmap";
            }
            else {
                bitmapID = "JavascriptBitmap";
            }
        }

        var blayout = new Atomic.UILayout();

        blayout.gravity = Atomic.UI_GRAVITY_LEFT;

        var spacer = new Atomic.UIWidget();
        spacer.rect = [0, 0, 8, 8];
        blayout.addChild(spacer);

        var button = new Atomic.UIButton();

        var lp = new Atomic.UILayoutParams;
        lp.height = 20;

        var fd = new Atomic.UIFontDescription();
        fd.id = "Vera";
        fd.size = 11;

        button.gravity = Atomic.UI_GRAVITY_LEFT;

        var image = new Atomic.UISkinImage(bitmapID);
        image.rect = [0, 0, 12, 12];
        image.gravity = Atomic.UI_GRAVITY_RIGHT;
        blayout.addChild(image);

        button.id = fullpath;
        button.layoutParams = lp;
        button.fontDescription = fd;
        button.text = text;
        button.skinBg = "TBButton.flat";
        blayout.addChild(button);

        return blayout;
    }


    private currentContentFolder: string;


}

export = ProjectFrame;
